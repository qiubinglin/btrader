#include "core/book.h"

#include <algorithm>
#include <iostream>
#include <numeric>
#include <sstream>
#include <stdexcept>
#include <unordered_set>

#include "infra/time.h"

namespace btra {

// ============================================================================
// PositionBook Implementation
// ============================================================================

PositionBook::PositionBook(const char *data, uint32_t len) {
    if (len < sizeof(uint32_t) * 2) {
        throw std::runtime_error("PositionBook data too short");
    }

    const char *ptr = data;
    uint32_t long_size = *reinterpret_cast<const uint32_t *>(ptr);
    ptr += sizeof(uint32_t);
    uint32_t short_size = *reinterpret_cast<const uint32_t *>(ptr);
    ptr += sizeof(uint32_t);

    // Validate data length
    uint32_t expected_size = sizeof(uint32_t) * 2 + long_size * (sizeof(uint32_t) + sizeof(Position)) +
                             short_size * (sizeof(uint32_t) + sizeof(Position));
    if (len < expected_size) {
        throw std::runtime_error("PositionBook data length mismatch");
    }

    // Load long positions
    for (uint32_t i = 0; i < long_size; ++i) {
        uint32_t key = *reinterpret_cast<const uint32_t *>(ptr);
        ptr += sizeof(uint32_t);
        Position position = *reinterpret_cast<const Position *>(ptr);
        ptr += sizeof(Position);
        long_positions[key] = position;
    }

    // Load short positions
    for (uint32_t i = 0; i < short_size; ++i) {
        uint32_t key = *reinterpret_cast<const uint32_t *>(ptr);
        ptr += sizeof(uint32_t);
        Position position = *reinterpret_cast<const Position *>(ptr);
        ptr += sizeof(Position);
        short_positions[key] = position;
    }
}

std::string PositionBook::to_string() const {
    std::string buf;
    size_t long_size = long_positions.size() * (sizeof(uint32_t) + sizeof(Position));
    size_t short_size = short_positions.size() * (sizeof(uint32_t) + sizeof(Position));
    size_t total_size = sizeof(uint32_t) * 2 + long_size + short_size;

    buf.resize(total_size);
    char *ptr = buf.data();

    // Write sizes
    *reinterpret_cast<uint32_t *>(ptr) = static_cast<uint32_t>(long_positions.size());
    ptr += sizeof(uint32_t);
    *reinterpret_cast<uint32_t *>(ptr) = static_cast<uint32_t>(short_positions.size());
    ptr += sizeof(uint32_t);

    // Write long positions
    for (const auto &[key, val] : long_positions) {
        *reinterpret_cast<uint32_t *>(ptr) = key;
        ptr += sizeof(uint32_t);
        *reinterpret_cast<Position *>(ptr) = val;
        ptr += sizeof(Position);
    }

    // Write short positions
    for (const auto &[key, val] : short_positions) {
        *reinterpret_cast<uint32_t *>(ptr) = key;
        ptr += sizeof(uint32_t);
        *reinterpret_cast<Position *>(ptr) = val;
        ptr += sizeof(Position);
    }

    return buf;
}

void PositionBook::set(const Position &position) {
    auto &positions = position.direction == enums::Direction::Long ? long_positions : short_positions;
    positions[hash_instrument(position.exchange_id, position.instrument_id)] = position;
}

const Position *PositionBook::get(const infra::Array<char, INSTRUMENT_ID_LEN> &instrument_id,
                                  const infra::Array<char, EXCHANGE_ID_LEN> &exchange_id,
                                  enums::Direction direction) const {
    const auto &positions = direction == enums::Direction::Long ? long_positions : short_positions;
    auto it = positions.find(hash_instrument(exchange_id, instrument_id));
    return it != positions.end() ? &it->second : nullptr;
}

Position *PositionBook::get(const infra::Array<char, INSTRUMENT_ID_LEN> &instrument_id,
                            const infra::Array<char, EXCHANGE_ID_LEN> &exchange_id, enums::Direction direction) {
    auto &positions = direction == enums::Direction::Long ? long_positions : short_positions;
    auto it = positions.find(hash_instrument(exchange_id, instrument_id));
    return it != positions.end() ? &it->second : nullptr;
}

std::vector<Position *> PositionBook::get_all_positions(const infra::Array<char, INSTRUMENT_ID_LEN> &instrument_id,
                                                        const infra::Array<char, EXCHANGE_ID_LEN> &exchange_id) {
    std::vector<Position *> result;
    uint32_t hash_key = hash_instrument(exchange_id, instrument_id);

    // Check long positions
    auto long_it = long_positions.find(hash_key);
    if (long_it != long_positions.end()) {
        result.push_back(&long_it->second);
    }

    // Check short positions
    auto short_it = short_positions.find(hash_key);
    if (short_it != short_positions.end()) {
        result.push_back(&short_it->second);
    }

    return result;
}

const std::unordered_map<uint32_t, Position> &PositionBook::get_positions_by_direction(
    enums::Direction direction) const {
    return direction == enums::Direction::Long ? long_positions : short_positions;
}

bool PositionBook::remove_position(const infra::Array<char, INSTRUMENT_ID_LEN> &instrument_id,
                                   const infra::Array<char, EXCHANGE_ID_LEN> &exchange_id, enums::Direction direction) {
    auto &positions = direction == enums::Direction::Long ? long_positions : short_positions;
    uint32_t hash_key = hash_instrument(exchange_id, instrument_id);
    auto it = positions.find(hash_key);
    if (it != positions.end()) {
        positions.erase(it);
        return true;
    }
    return false;
}

void PositionBook::clear() {
    long_positions.clear();
    short_positions.clear();
}

bool PositionBook::has_positions() const { return !long_positions.empty() || !short_positions.empty(); }

void PositionBook::update(const Trade &trade) {
    try {
        // Calculate hash key for the instrument
        uint32_t hash_key = hash_instrument(trade.exchange_id, trade.instrument_id);

        // Determine position direction based on trade side and offset
        enums::Direction position_direction;
        if (trade.side == enums::Side::Buy) {
            // Buy side: Long position for Open, Short position for Close
            position_direction =
                (trade.offset == enums::Offset::Open) ? enums::Direction::Long : enums::Direction::Short;
        } else if (trade.side == enums::Side::Sell) {
            // Sell side: Short position for Open, Long position for Close
            position_direction =
                (trade.offset == enums::Offset::Open) ? enums::Direction::Short : enums::Direction::Long;
        } else {
            // For other side types, skip position update
            std::cout << "Warning: Unsupported trade side for position update: " << static_cast<int>(trade.side)
                      << std::endl;
            return;
        }

        // Get the appropriate position map
        auto &positions = (position_direction == enums::Direction::Long) ? long_positions : short_positions;

        // Find existing position or create new one
        auto it = positions.find(hash_key);
        if (it == positions.end()) {
            // Create new position
            Position new_position;
            new_position.update_time = trade.trade_time;
            new_position.trading_day = trade.trading_day;
            new_position.instrument_id = trade.instrument_id;
            new_position.instrument_type = trade.instrument_type;
            new_position.exchange_id = trade.exchange_id;
            new_position.direction = position_direction;
            new_position.volume = trade.volume;
            new_position.position_cost_price = trade.price;
            new_position.unrealized_pnl = 0.0;

            positions[hash_key] = new_position;

            std::cout << "Created new " << (position_direction == enums::Direction::Long ? "long" : "short")
                      << " position for " << trade.instrument_id.to_string() << " with volume " << trade.volume
                      << " at price " << trade.price << std::endl;
        } else {
            // Update existing position
            Position &position = it->second;

            // Calculate new average cost price
            double total_cost = position.position_cost_price * position.volume;
            double new_cost = trade.price * trade.volume;
            int64_t total_volume = position.volume + trade.volume;

            if (total_volume > static_cast<int64_t>(0)) {
                position.position_cost_price = (total_cost + new_cost) / total_volume;
            }

            // Update volume
            if (trade.offset == enums::Offset::Open) {
                // Opening position: add volume
                position.volume += trade.volume;
            } else {
                // Closing position: reduce volume
                position.volume -= trade.volume;

                // Check if position is fully closed
                if (position.volume <= static_cast<int64_t>(0)) {
                    // Position fully closed, remove it
                    positions.erase(it);
                    std::cout << "Position fully closed for " << trade.instrument_id.to_string()
                              << " (direction: " << (position_direction == enums::Direction::Long ? "long" : "short")
                              << ")" << std::endl;
                    return;
                }
            }

            // Update timestamp
            position.update_time = trade.trade_time;

            std::cout << "Updated " << (position_direction == enums::Direction::Long ? "long" : "short")
                      << " position for " << trade.instrument_id.to_string() << " - New volume: " << position.volume
                      << ", New cost price: " << position.position_cost_price << std::endl;
        }

    } catch (const std::exception &e) {
        std::cerr << "Error updating position book with trade: " << e.what() << std::endl;
    }
}

double PositionBook::unrealized_pnl() const {
    double retval = 0.0;

    // Sum up unrealized PnL from long positions
    for (const auto &[_, position] : long_positions) {
        retval += position.unrealized_pnl;
    }

    // Sum up unrealized PnL from short positions
    for (const auto &[_, position] : short_positions) {
        retval += position.unrealized_pnl;
    }

    return retval;
}

// ============================================================================
// CommissionBook Implementation
// ============================================================================

void CommissionBook::set(const Commission &commission) {
    commissions_[infra::hash_str_32(commission.product_id)] = commission;
}

const Commission *CommissionBook::get(const infra::Array<char, PRODUCT_ID_LEN> &product_id) const {
    auto it = commissions_.find(infra::hash_str_32(product_id));
    return it != commissions_.end() ? &it->second : nullptr;
}

bool CommissionBook::contains(const infra::Array<char, PRODUCT_ID_LEN> &product_id) const {
    return commissions_.find(infra::hash_str_32(product_id)) != commissions_.end();
}

void CommissionBook::set_batch(const std::vector<Commission> &commissions) {
    for (const auto &commission : commissions) {
        set(commission);
    }
}

Commission CommissionBook::get_with_default(const infra::Array<char, PRODUCT_ID_LEN> &product_id,
                                            const Commission &default_commission) const {
    const Commission *found = get(product_id);
    return found ? *found : default_commission;
}

// ============================================================================
// InstrumentBook Implementation
// ============================================================================

void InstrumentBook::set(const Instrument &instrument) {
    instruments_[hash_instrument(instrument.exchange_id, instrument.instrument_id)] = instrument;
}

const Instrument *InstrumentBook::get(const infra::Array<char, INSTRUMENT_ID_LEN> &instrument_id,
                                      const infra::Array<char, EXCHANGE_ID_LEN> &exchange_id) const {
    auto it = instruments_.find(hash_instrument(exchange_id, instrument_id));
    return it != instruments_.end() ? &it->second : nullptr;
}

bool InstrumentBook::contains(const infra::Array<char, INSTRUMENT_ID_LEN> &instrument_id,
                              const infra::Array<char, EXCHANGE_ID_LEN> &exchange_id) const {
    return instruments_.find(hash_instrument(exchange_id, instrument_id)) != instruments_.end();
}

void InstrumentBook::set_batch(const std::vector<Instrument> &instruments) {
    for (const auto &instrument : instruments) {
        set(instrument);
    }
}

std::vector<const Instrument *> InstrumentBook::find_by_exchange(
    const infra::Array<char, EXCHANGE_ID_LEN> &exchange_id) const {
    std::vector<const Instrument *> result;
    result.reserve(instruments_.size()); // Reserve space for potential worst case

    for (const auto &[key, instrument] : instruments_) {
        if (instrument.exchange_id.to_string() == exchange_id.to_string()) {
            result.push_back(&instrument);
        }
    }

    return result;
}

// ============================================================================
// OrderInputBook Implementation
// ============================================================================

void OrderInputBook::set(const OrderInput &order_input) { order_inputs_[order_input.order_id] = order_input; }

const OrderInput *OrderInputBook::get(uint64_t order_id) const {
    auto it = order_inputs_.find(order_id);
    return it != order_inputs_.end() ? &it->second : nullptr;
}

bool OrderInputBook::contains(uint64_t order_id) const { return order_inputs_.find(order_id) != order_inputs_.end(); }

void OrderInputBook::set_batch(const std::vector<OrderInput> &order_inputs) {
    for (const auto &order_input : order_inputs) {
        set(order_input);
    }
}

std::vector<const OrderInput *> OrderInputBook::find_by_instrument(
    const infra::Array<char, INSTRUMENT_ID_LEN> &instrument_id,
    const infra::Array<char, EXCHANGE_ID_LEN> &exchange_id) const {
    std::vector<const OrderInput *> result;
    result.reserve(order_inputs_.size()); // Reserve space for potential worst case

    for (const auto &[key, order_input] : order_inputs_) {
        if (order_input.instrument_id.to_string() == instrument_id.to_string() &&
            order_input.exchange_id.to_string() == exchange_id.to_string()) {
            result.push_back(&order_input);
        }
    }

    return result;
}

// ============================================================================
// OrderBook Implementation
// ============================================================================

void OrderBook::set(const Order &order) { orders_[order.order_id] = order; }

const Order *OrderBook::get(uint64_t order_id) const {
    auto it = orders_.find(order_id);
    return it != orders_.end() ? &it->second : nullptr;
}

bool OrderBook::contains(uint64_t order_id) const { return orders_.find(order_id) != orders_.end(); }

void OrderBook::set_batch(const std::vector<Order> &orders) {
    for (const auto &order : orders) {
        set(order);
    }
}

std::vector<const Order *> OrderBook::find_by_instrument(const infra::Array<char, INSTRUMENT_ID_LEN> &instrument_id,
                                                         const infra::Array<char, EXCHANGE_ID_LEN> &exchange_id) const {
    std::vector<const Order *> result;
    result.reserve(orders_.size()); // Reserve space for potential worst case

    for (const auto &[key, order] : orders_) {
        if (order.instrument_id.to_string() == instrument_id.to_string() &&
            order.exchange_id.to_string() == exchange_id.to_string()) {
            result.push_back(&order);
        }
    }

    return result;
}

std::vector<const Order *> OrderBook::find_by_status(enums::OrderStatus status) const {
    std::vector<const Order *> result;
    result.reserve(orders_.size()); // Reserve space for potential worst case

    for (const auto &[key, order] : orders_) {
        if (order.status == status) {
            result.push_back(&order);
        }
    }

    return result;
}

// ============================================================================
// TradeBook Implementation
// ============================================================================

void TradeBook::set(const Trade &trade) { trades_[trade.trade_id] = trade; }

const Trade *TradeBook::get(uint64_t trade_id) const {
    auto it = trades_.find(trade_id);
    return it != trades_.end() ? &it->second : nullptr;
}

bool TradeBook::contains(uint64_t trade_id) const { return trades_.find(trade_id) != trades_.end(); }

void TradeBook::set_batch(const std::vector<Trade> &trades) {
    for (const auto &trade : trades) {
        set(trade);
    }
}

std::vector<const Trade *> TradeBook::find_by_instrument(const infra::Array<char, INSTRUMENT_ID_LEN> &instrument_id,
                                                         const infra::Array<char, EXCHANGE_ID_LEN> &exchange_id) const {
    std::vector<const Trade *> result;
    result.reserve(trades_.size()); // Reserve space for potential worst case

    for (const auto &[key, trade] : trades_) {
        if (trade.instrument_id.to_string() == instrument_id.to_string() &&
            trade.exchange_id.to_string() == exchange_id.to_string()) {
            result.push_back(&trade);
        }
    }

    return result;
}

std::vector<const Trade *> TradeBook::find_by_time_range(uint64_t start_time, uint64_t end_time) const {
    std::vector<const Trade *> result;
    result.reserve(trades_.size()); // Reserve space for potential worst case

    for (const auto &[key, trade] : trades_) {
        if (trade.trade_time >= static_cast<int64_t>(start_time) &&
            trade.trade_time <= static_cast<int64_t>(end_time)) {
            result.push_back(&trade);
        }
    }

    return result;
}

// ============================================================================
// Book Implementation
// ============================================================================

void Book::update(const Trade &trade) {
    try {
        // Update positions based on trade
        positions.update(trade);

        // Update asset information
        // Note: This is a simplified implementation. In a real system,
        // you would need to handle commission, tax, and other costs
        if (trade.side == enums::Side::Buy) {
            // Buying: reduce available cash
            asset.avail -= trade.price * trade.volume;
        } else if (trade.side == enums::Side::Sell) {
            // Selling: increase available cash
            asset.avail += trade.price * trade.volume;
        }

        // Apply commission and tax
        asset.avail -= trade.commission;
        asset.avail -= trade.tax;

        std::cout << "Updated book with trade: " << trade.instrument_id.to_string()
                  << " - Side: " << (trade.side == enums::Side::Buy ? "BUY" : "SELL") << ", Volume: " << trade.volume
                  << ", Price: " << trade.price << ", Commission: " << trade.commission << ", Tax: " << trade.tax
                  << std::endl;

    } catch (const std::exception &e) {
        std::cerr << "Error updating book with trade: " << e.what() << std::endl;
    }
}

void Book::update(const Bar &bar) {
    try {
        // Validate input parameters - check if instrument_id and exchange_id are not empty strings
        if (bar.instrument_id[0] == '\0' || bar.exchange_id[0] == '\0') {
            std::cout << "Warning: Invalid bar data - empty instrument_id or exchange_id" << std::endl;
            return;
        }

        // Check if we have any positions for this instrument
        auto long_position = positions.get(bar.instrument_id, bar.exchange_id, enums::Direction::Long);
        auto short_position = positions.get(bar.instrument_id, bar.exchange_id, enums::Direction::Short);

        // Update long position if exists and has volume
        if (long_position != nullptr && long_position->volume > static_cast<int64_t>(0)) {
            update_position_pnl(*long_position, bar);
        }

        // Update short position if exists and has volume
        if (short_position != nullptr && short_position->volume > static_cast<int64_t>(0)) {
            update_position_pnl(*short_position, bar);
        }

        // Update asset information if we have any positions
        if (long_position != nullptr || short_position != nullptr) {
            update_asset_from_positions(bar);
        }

    } catch (const std::exception &e) {
        std::cerr << "Error updating book with bar data: " << e.what() << std::endl;
    }
}

double Book::asset_price() const { return asset.avail + positions.unrealized_pnl(); }

double Book::total_unrealized_pnl() const { return positions.unrealized_pnl(); }

double Book::total_realized_pnl() const {
    double total = 0.0;
    for (const auto &[_, trade] : trades) {
        // Calculate realized PnL based on trade details
        // This is a simplified calculation - in practice you'd need more complex logic
        if (trade.offset == enums::Offset::Close) {
            // For closing trades, calculate PnL
            double pnl = 0.0;
            if (trade.side == enums::Side::Sell) {
                // Selling to close long position
                pnl = (trade.price - trade.price) * trade.volume; // Simplified
            } else {
                // Buying to close short position
                pnl = (trade.price - trade.price) * trade.volume; // Simplified
            }
            total += pnl;
        }
    }
    return total;
}

double Book::total_commission() const {
    double total = 0.0;
    for (const auto &[_, trade] : trades) {
        total += trade.commission;
    }
    return total;
}

double Book::total_tax() const {
    double total = 0.0;
    for (const auto &[_, trade] : trades) {
        total += trade.tax;
    }
    return total;
}

bool Book::validate_consistency() const { return validate_positions() && validate_orders() && validate_trades(); }

void Book::clear() {
    commissions.clear();
    instruments.clear();
    asset = {};
    asset_margin = {};
    positions.clear();
    order_inputs.clear();
    orders.clear();
    trades.clear();
}

// Helper method to update position's unrealized PnL based on current market price
void Book::update_position_pnl(Position &position, const Bar &bar) {
    try {
        // Calculate direction factor: 1 for long, -1 for short
        int factor = (position.direction == enums::Direction::Long) ? 1 : -1;

        // Calculate unrealized PnL: volume * (current_price - cost_price) * direction_factor
        double unrealized_pnl = position.volume * (bar.close - position.position_cost_price) * factor;

        // Update position's unrealized PnL
        position.unrealized_pnl = unrealized_pnl;

        // Update position's last update time
        position.update_time = bar.end_time;

        // Log position update for debugging
        std::cout << "Updated " << (position.direction == enums::Direction::Long ? "long" : "short")
                  << " position PnL for " << position.instrument_id.to_string() << " - Volume: " << position.volume
                  << ", Cost Price: " << position.position_cost_price << ", Current Price: " << bar.close
                  << ", Unrealized PnL: " << unrealized_pnl << std::endl;

    } catch (const std::exception &e) {
        std::cerr << "Error updating position PnL: " << e.what() << std::endl;
    }
}

// Helper method to update asset information based on current positions
void Book::update_asset_from_positions(const Bar &bar) {
    try {
        // Calculate total unrealized PnL from all positions
        double total_unrealized_pnl = positions.unrealized_pnl();

        // Update asset's last update time
        asset.update_time = bar.end_time;

        // Log asset update for debugging
        std::cout << "Updated asset - Available: " << asset.avail << ", Total Unrealized PnL: " << total_unrealized_pnl
                  << ", Asset Price: " << asset_price() << std::endl;

    } catch (const std::exception &e) {
        std::cerr << "Error updating asset from positions: " << e.what() << std::endl;
    }
}

// Validation helper methods
bool Book::validate_positions() const {
    // Check for negative volumes
    for (const auto &[_, position] : positions.long_positions) {
        if (position.volume < 0) {
            std::cerr << "Invalid position: negative volume in long position" << std::endl;
            return false;
        }
    }

    for (const auto &[_, position] : positions.short_positions) {
        if (position.volume < 0) {
            std::cerr << "Invalid position: negative volume in short position" << std::endl;
            return false;
        }
    }

    return true;
}

bool Book::validate_orders() const {
    // Check for duplicate order IDs
    std::unordered_set<uint64_t> order_ids;
    for (const auto &[order_id, _] : orders) {
        if (!order_ids.insert(order_id).second) {
            std::cerr << "Invalid order: duplicate order ID " << order_id << std::endl;
            return false;
        }
    }

    return true;
}

bool Book::validate_trades() const {
    // Check for duplicate trade IDs
    std::unordered_set<uint64_t> trade_ids;
    for (const auto &[trade_id, _] : trades) {
        if (!trade_ids.insert(trade_id).second) {
            std::cerr << "Invalid trade: duplicate trade ID " << trade_id << std::endl;
            return false;
        }
    }

    return true;
}

} // namespace btra