#include "core/book.h"

#include <iostream>

#include "infra/time.h"

namespace btra {

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
        if (long_position != nullptr && long_position->volume > 0) {
            update_position_pnl(*long_position, bar);
        }

        // Update short position if exists and has volume
        if (short_position != nullptr && short_position->volume > 0) {
            update_position_pnl(*short_position, bar);
        }

        // Update asset information if we have any positions
        if (long_position != nullptr || short_position != nullptr) {
            update_asset_from_positions(bar);
        }

    } catch (const std::exception& e) {
        std::cerr << "Error updating book with bar data: " << e.what() << std::endl;
    }
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
                  << " position PnL for " << position.instrument_id.to_string()
                  << " - Volume: " << position.volume
                  << ", Cost Price: " << position.position_cost_price
                  << ", Current Price: " << bar.close
                  << ", Unrealized PnL: " << unrealized_pnl << std::endl;
                  
    } catch (const std::exception& e) {
        std::cerr << "Error updating position PnL: " << e.what() << std::endl;
    }
}

// Helper method to update asset information based on current positions
void Book::update_asset_from_positions(const Bar &bar) {
    try {
        // Calculate total unrealized PnL from all positions
        double total_unrealized_pnl = 0.0;
        
        // Sum up unrealized PnL from long positions
        for (const auto& [hash_key, position] : positions.long_positions) {
            if (position.volume > 0) {
                total_unrealized_pnl += position.unrealized_pnl;
            }
        }
        
        // Sum up unrealized PnL from short positions
        for (const auto& [hash_key, position] : positions.short_positions) {
            if (position.volume > 0) {
                total_unrealized_pnl += position.unrealized_pnl;
            }
        }
        
        // Update asset's last update time
        asset.update_time = bar.end_time;
        
        // Log asset update for debugging
        std::cout << "Updated asset - Available: " << asset.avail
                  << ", Total Unrealized PnL: " << total_unrealized_pnl
                  << ", Asset Price: " << asset_price() << std::endl;
                  
    } catch (const std::exception& e) {
        std::cerr << "Error updating asset from positions: " << e.what() << std::endl;
    }
}

double Book::asset_price() const { return asset.avail + positions.unrealized_pnl(); }

void PositionBook::set(const Position &position) {
    auto &positions = position.direction == enums::Direction::Long ? long_positions : short_positions;
    positions[hash_instrument(position.exchange_id, position.instrument_id)] = position;
}

Position *PositionBook::get(const infra::Array<char, INSTRUMENT_ID_LEN> &instrument_id,
                            const infra::Array<char, EXCHANGE_ID_LEN> &exchange_id, enums::Direction direction) {
    auto &positions = direction == enums::Direction::Long ? long_positions : short_positions;
    auto it = positions.find(hash_instrument(exchange_id, instrument_id));
    return it != positions.end() ? &it->second : nullptr;
}

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

            if (total_volume > 0) {
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
                if (position.volume <= 0) {
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
    double retval = 0;
    for (const auto &[_, position] : long_positions) {
        retval += position.position_cost_price * position.volume + position.unrealized_pnl;
    }

    for (const auto &[_, position] : short_positions) {
        retval += position.position_cost_price * position.volume + position.unrealized_pnl;
    }
    return retval;
}

} // namespace btra