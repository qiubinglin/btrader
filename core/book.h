#pragma once

#include <unordered_map>
#include <vector>

#include "core/hashid.h"
#include "core/types.h"

namespace btra {

/**
 * @brief Position book that manages long and short positions for instruments
 * key = hash_instrument(exchange_id, instrument_id)
 * This structure maintains separate maps for long and short positions,
 * allowing efficient position management and PnL calculations.
 */
struct PositionBook {
    UNFIXED_DATA_BODY(PositionBook)

    std::unordered_map<uint32_t, Position> long_positions;
    std::unordered_map<uint32_t, Position> short_positions;

    /**
     * @brief Set or update a position in the book
     * @param position The position to set
     */
    void set(const Position &position);

    /**
     * @brief Get a position by instrument and exchange IDs (const version)
     * @param instrument_id The instrument identifier
     * @param exchange_id The exchange identifier
     * @param direction The position direction (Long/Short)
     * @return Pointer to the position if found, nullptr otherwise
     */
    const Position *get(const infra::Array<char, INSTRUMENT_ID_LEN> &instrument_id,
                        const infra::Array<char, EXCHANGE_ID_LEN> &exchange_id, enums::Direction direction) const;

    /**
     * @brief Get a position by instrument and exchange IDs (non-const version)
     * @param instrument_id The instrument identifier
     * @param exchange_id The exchange identifier
     * @param direction The position direction (Long/Short)
     * @return Pointer to the position if found, nullptr otherwise
     */
    Position *get(const infra::Array<char, INSTRUMENT_ID_LEN> &instrument_id,
                  const infra::Array<char, EXCHANGE_ID_LEN> &exchange_id, enums::Direction direction);

    /**
     * @brief Get all positions for a specific instrument
     * @param instrument_id The instrument identifier
     * @param exchange_id The exchange identifier
     * @return Vector of pointers to positions (can be empty)
     */
    std::vector<Position *> get_all_positions(const infra::Array<char, INSTRUMENT_ID_LEN> &instrument_id,
                                              const infra::Array<char, EXCHANGE_ID_LEN> &exchange_id);

    /**
     * @brief Update positions based on a transaction
     * @param transaction The transaction to process
     */
    void update(const Transaction &transaction);

    /**
     * @brief Update positions based on a trade
     * @param trade The trade to process
     */
    void update(const Trade &trade);

    /**
     * @brief Calculate total unrealized PnL across all positions
     * @return Total unrealized PnL
     */
    double unrealized_pnl() const;

    /**
     * @brief Check if the book has any open positions
     * @return true if there are open positions, false otherwise
     */
    bool has_positions() const;

    /**
     * @brief Get total position count
     * @return Total number of positions
     */
    size_t total_positions() const { return long_positions.size() + short_positions.size(); }

    /**
     * @brief Get positions by direction
     * @param direction The position direction
     * @return Reference to the positions map for the specified direction
     */
    const std::unordered_map<uint32_t, Position> &get_positions_by_direction(enums::Direction direction) const;

    /**
     * @brief Remove a position by instrument and exchange IDs
     * @param instrument_id The instrument identifier
     * @param exchange_id The exchange identifier
     * @param direction The position direction
     * @return true if position was removed, false if not found
     */
    bool remove_position(const infra::Array<char, INSTRUMENT_ID_LEN> &instrument_id,
                         const infra::Array<char, EXCHANGE_ID_LEN> &exchange_id, enums::Direction direction);

    /**
     * @brief Clear all positions
     */
    void clear();
};

/**
 * @brief Commission book that manages commission rates for products
 * key = hash_str_32(product_id) or key = hash_product(exchange_id, product_id)
 * Uses composition instead of inheritance for better design and safety.
 */
class CommissionBook {
private:
    std::unordered_map<uint32_t, Commission> commissions_;

public:
    // Type aliases for better readability
    using key_type = uint32_t;
    using mapped_type = Commission;
    using container_type = std::unordered_map<key_type, mapped_type>;
    using iterator = container_type::iterator;
    using const_iterator = container_type::const_iterator;

    /**
     * @brief Set or update a commission rate
     * @param commission The commission to set
     */
    void set(const Commission &commission);

    /**
     * @brief Get commission rate for a product
     * @param product_id The product identifier
     * @return Pointer to commission if found, nullptr otherwise
     */
    const Commission *get(const infra::Array<char, PRODUCT_ID_LEN> &product_id) const;

    /**
     * @brief Check if commission exists for a product
     * @param product_id The product identifier
     * @return true if commission exists, false otherwise
     */
    bool contains(const infra::Array<char, PRODUCT_ID_LEN> &product_id) const;

    /**
     * @brief Get the number of commission entries
     * @return Number of commission entries
     */
    size_t size() const { return commissions_.size(); }

    /**
     * @brief Check if the book is empty
     * @return true if empty, false otherwise
     */
    bool empty() const { return commissions_.empty(); }

    /**
     * @brief Clear all commission entries
     */
    void clear() { commissions_.clear(); }

    // Iterator support
    iterator begin() { return commissions_.begin(); }
    iterator end() { return commissions_.end(); }
    const_iterator begin() const { return commissions_.begin(); }
    const_iterator end() const { return commissions_.end(); }
    const_iterator cbegin() const { return commissions_.cbegin(); }
    const_iterator cend() const { return commissions_.cend(); }

    /**
     * @brief Batch set multiple commissions
     * @param commissions Vector of commissions to set
     */
    void set_batch(const std::vector<Commission> &commissions);

    /**
     * @brief Get commission with fallback to default
     * @param product_id The product identifier
     * @param default_commission Default commission to return if not found
     * @return Commission value or default
     */
    Commission get_with_default(const infra::Array<char, PRODUCT_ID_LEN> &product_id,
                                const Commission &default_commission) const;
};

/**
 * @brief Instrument book that manages instrument information
 * key = hash_instrument(exchange_id, instrument_id)
 * Uses composition instead of inheritance for better design and safety.
 */
class InstrumentBook {
private:
    std::unordered_map<uint32_t, Instrument> instruments_;

public:
    // Type aliases for better readability
    using key_type = uint32_t;
    using mapped_type = Instrument;
    using container_type = std::unordered_map<key_type, mapped_type>;
    using iterator = container_type::iterator;
    using const_iterator = container_type::const_iterator;

    /**
     * @brief Set or update an instrument
     * @param instrument The instrument to set
     */
    void set(const Instrument &instrument);

    /**
     * @brief Get instrument by IDs
     * @param instrument_id The instrument identifier
     * @param exchange_id The exchange identifier
     * @return Pointer to instrument if found, nullptr otherwise
     */
    const Instrument *get(const infra::Array<char, INSTRUMENT_ID_LEN> &instrument_id,
                          const infra::Array<char, EXCHANGE_ID_LEN> &exchange_id) const;

    /**
     * @brief Check if instrument exists
     * @param instrument_id The instrument identifier
     * @param exchange_id The exchange identifier
     * @return true if instrument exists, false otherwise
     */
    bool contains(const infra::Array<char, INSTRUMENT_ID_LEN> &instrument_id,
                  const infra::Array<char, EXCHANGE_ID_LEN> &exchange_id) const;

    /**
     * @brief Get the number of instruments
     * @return Number of instruments
     */
    size_t size() const { return instruments_.size(); }

    /**
     * @brief Check if the book is empty
     * @return true if empty, false otherwise
     */
    bool empty() const { return instruments_.empty(); }

    /**
     * @brief Clear all instruments
     */
    void clear() { instruments_.clear(); }

    // Iterator support
    iterator begin() { return instruments_.begin(); }
    iterator end() { return instruments_.end(); }
    const_iterator begin() const { return instruments_.begin(); }
    const_iterator end() const { return instruments_.end(); }
    const_iterator cbegin() const { return instruments_.cbegin(); }
    const_iterator cend() const { return instruments_.cend(); }

    /**
     * @brief Batch set multiple instruments
     * @param instruments Vector of instruments to set
     */
    void set_batch(const std::vector<Instrument> &instruments);

    /**
     * @brief Find instruments by exchange
     * @param exchange_id The exchange identifier
     * @return Vector of instruments for the specified exchange
     */
    std::vector<const Instrument *> find_by_exchange(const infra::Array<char, EXCHANGE_ID_LEN> &exchange_id) const;
};

/**
 * @brief Order input book that manages order input data
 *
 * Uses composition instead of inheritance for better design and safety.
 */
class OrderInputBook {
private:
    std::unordered_map<uint64_t, OrderInput> order_inputs_;

public:
    // Type aliases for better readability
    using key_type = uint64_t;
    using mapped_type = OrderInput;
    using container_type = std::unordered_map<key_type, mapped_type>;
    using iterator = container_type::iterator;
    using const_iterator = container_type::const_iterator;

    /**
     * @brief Set or update an order input
     * @param order_input The order input to set
     */
    void set(const OrderInput &order_input);

    /**
     * @brief Get order input by order ID
     * @param order_id The order identifier
     * @return Pointer to order input if found, nullptr otherwise
     */
    const OrderInput *get(uint64_t order_id) const;

    /**
     * @brief Check if order input exists
     * @param order_id The order identifier
     * @return true if order input exists, false otherwise
     */
    bool contains(uint64_t order_id) const;

    /**
     * @brief Get the number of order inputs
     * @return Number of order inputs
     */
    size_t size() const { return order_inputs_.size(); }

    /**
     * @brief Check if the book is empty
     * @return true if empty, false otherwise
     */
    bool empty() const { return order_inputs_.empty(); }

    /**
     * @brief Clear all order inputs
     */
    void clear() { order_inputs_.clear(); }

    // Iterator support
    iterator begin() { return order_inputs_.begin(); }
    iterator end() { return order_inputs_.end(); }
    const_iterator begin() const { return order_inputs_.begin(); }
    const_iterator end() const { return order_inputs_.end(); }
    const_iterator cbegin() const { return order_inputs_.cbegin(); }
    const_iterator cend() const { return order_inputs_.cend(); }

    /**
     * @brief Batch set multiple order inputs
     * @param order_inputs Vector of order inputs to set
     */
    void set_batch(const std::vector<OrderInput> &order_inputs);

    /**
     * @brief Find order inputs by instrument
     * @param instrument_id The instrument identifier
     * @param exchange_id The exchange identifier
     * @return Vector of order inputs for the specified instrument
     */
    std::vector<const OrderInput *> find_by_instrument(const infra::Array<char, INSTRUMENT_ID_LEN> &instrument_id,
                                                       const infra::Array<char, EXCHANGE_ID_LEN> &exchange_id) const;
};

/**
 * @brief Order book that manages order information
 *
 * Uses composition instead of inheritance for better design and safety.
 */
class OrderBook {
private:
    std::unordered_map<uint64_t, Order> orders_;

public:
    // Type aliases for better readability
    using key_type = uint64_t;
    using mapped_type = Order;
    using container_type = std::unordered_map<key_type, mapped_type>;
    using iterator = container_type::iterator;
    using const_iterator = container_type::const_iterator;

    /**
     * @brief Set or update an order
     * @param order The order to set
     */
    void set(const Order &order);

    /**
     * @brief Get order by order ID
     * @param order_id The order identifier
     * @return Pointer to order if found, nullptr otherwise
     */
    const Order *get(uint64_t order_id) const;

    /**
     * @brief Check if order exists
     * @param order_id The order identifier
     * @return true if order exists, false otherwise
     */
    bool contains(uint64_t order_id) const;

    /**
     * @brief Get the number of orders
     * @return Number of orders
     */
    size_t size() const { return orders_.size(); }

    /**
     * @brief Check if the book is empty
     * @return true if empty, false otherwise
     */
    bool empty() const { return orders_.empty(); }

    /**
     * @brief Clear all orders
     */
    void clear() { orders_.clear(); }

    // Iterator support
    iterator begin() { return orders_.begin(); }
    iterator end() { return orders_.end(); }
    const_iterator begin() const { return orders_.begin(); }
    const_iterator end() const { return orders_.end(); }
    const_iterator cbegin() const { return orders_.cbegin(); }
    const_iterator cend() const { return orders_.cend(); }

    /**
     * @brief Batch set multiple orders
     * @param orders Vector of orders to set
     */
    void set_batch(const std::vector<Order> &orders);

    /**
     * @brief Find orders by instrument
     * @param instrument_id The instrument identifier
     * @param exchange_id The exchange identifier
     * @return Vector of orders for the specified instrument
     */
    std::vector<const Order *> find_by_instrument(const infra::Array<char, INSTRUMENT_ID_LEN> &instrument_id,
                                                  const infra::Array<char, EXCHANGE_ID_LEN> &exchange_id) const;

    /**
     * @brief Find orders by status
     * @param status The order status to filter by
     * @return Vector of orders with the specified status
     */
    std::vector<const Order *> find_by_status(enums::OrderStatus status) const;
};

/**
 * @brief Trade book that manages trade information
 *
 * Uses composition instead of inheritance for better design and safety.
 */
class TradeBook {
private:
    std::unordered_map<uint64_t, Trade> trades_;

public:
    // Type aliases for better readability
    using key_type = uint64_t;
    using mapped_type = Trade;
    using container_type = std::unordered_map<key_type, mapped_type>;
    using iterator = container_type::iterator;
    using const_iterator = container_type::const_iterator;

    /**
     * @brief Set or update a trade
     * @param trade The trade to set
     */
    void set(const Trade &trade);

    /**
     * @brief Get trade by trade ID
     * @param trade_id The trade identifier
     * @return Pointer to trade if found, nullptr otherwise
     */
    const Trade *get(uint64_t trade_id) const;

    /**
     * @brief Check if trade exists
     * @param trade_id The trade identifier
     * @return true if trade exists, false otherwise
     */
    bool contains(uint64_t trade_id) const;

    /**
     * @brief Get the number of trades
     * @return Number of trades
     */
    size_t size() const { return trades_.size(); }

    /**
     * @brief Check if the book is empty
     * @return true if empty, false otherwise
     */
    bool empty() const { return trades_.empty(); }

    /**
     * @brief Clear all trades
     */
    void clear() { trades_.clear(); }

    // Iterator support
    iterator begin() { return trades_.begin(); }
    iterator end() { return trades_.end(); }
    const_iterator begin() const { return trades_.begin(); }
    const_iterator end() const { return trades_.end(); }
    const_iterator cbegin() const { return trades_.cbegin(); }
    const_iterator cend() const { return trades_.cend(); }

    /**
     * @brief Batch set multiple trades
     * @param trades Vector of trades to set
     */
    void set_batch(const std::vector<Trade> &trades);

    /**
     * @brief Find trades by instrument
     * @param instrument_id The instrument identifier
     * @param exchange_id The exchange identifier
     * @return Vector of trades for the specified instrument
     */
    std::vector<const Trade *> find_by_instrument(const infra::Array<char, INSTRUMENT_ID_LEN> &instrument_id,
                                                  const infra::Array<char, EXCHANGE_ID_LEN> &exchange_id) const;

    /**
     * @brief Find trades by time range
     * @param start_time Start time (inclusive)
     * @param end_time End time (inclusive)
     * @return Vector of trades within the specified time range
     */
    std::vector<const Trade *> find_by_time_range(uint64_t start_time, uint64_t end_time) const;
};

/**
 * @brief Main book structure that holds all trading data
 *
 * This structure manages all trading-related data including:
 * - Commission rates for products
 * - Instrument information
 * - Asset and margin data
 * - Position information
 * - Order and trade data
 *
 * The book provides methods to update data based on market events
 * and calculate current asset values including unrealized PnL.
 */
struct Book {
    CommissionBook commissions;
    InstrumentBook instruments;

    /* For the trader, only total asset is meaningful, so save all account assets into one. */
    Asset asset = {};

    AssetMargin asset_margin = {}; /* Need it? */

    PositionBook positions = {};
    OrderInputBook order_inputs = {};
    OrderBook orders = {};
    TradeBook trades = {};

    /**
     * @brief Update book data based on a trade
     * @param trade The trade to process
     */
    void update(const Trade &trade);

    /**
     * @brief Update book data based on market bar data
     * @param bar The market bar data to process
     */
    void update(const Bar &bar);

    /**
     * @brief Calculate current total asset value including unrealized PnL
     * @return Total asset value
     */
    double asset_price() const;

    /**
     * @brief Get total unrealized PnL from all positions
     * @return Total unrealized PnL
     */
    double total_unrealized_pnl() const;

    /**
     * @brief Get total realized PnL from all trades
     * @return Total realized PnL
     */
    double total_realized_pnl() const;

    /**
     * @brief Get total commission paid
     * @return Total commission amount
     */
    double total_commission() const;

    /**
     * @brief Get total tax paid
     * @return Total tax amount
     */
    double total_tax() const;

    /**
     * @brief Validate book data consistency
     * @return true if data is consistent, false otherwise
     */
    bool validate_consistency() const;

    /**
     * @brief Clear all book data
     */
    void clear();

private:
    // Helper methods for updating positions and assets
    void update_position_pnl(Position &position, const Bar &bar);
    void update_asset_from_positions(const Bar &bar);

    // Validation helpers
    bool validate_positions() const;
    bool validate_orders() const;
    bool validate_trades() const;
};

} // namespace btra