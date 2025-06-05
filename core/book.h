#pragma once

#include "core/types.h"
#include "infra/hash.h"

namespace btra {

inline uint32_t hash_instrument(const infra::Array<char, EXCHANGE_ID_LEN> &exchange_id,
                                const infra::Array<char, INSTRUMENT_ID_LEN> &instrument_id) {
    return infra::hash_str_32(instrument_id) ^ infra::hash_str_32(exchange_id);
}

inline uint32_t hash_product(const infra::Array<char, EXCHANGE_ID_LEN> &exchange_id,
                             const infra::Array<char, PRODUCT_ID_LEN> &product_id) {
    return infra::hash_str_32(product_id) ^ infra::hash_str_32(exchange_id);
}

// key = hash_str_32(product_id) or key = hash_product(exchange_id, product_id)
typedef std::unordered_map<uint32_t, Commission> CommissionBook;

// key = hash_instrument(exchange_id, instrument_id)
typedef std::unordered_map<uint32_t, Instrument> InstrumentBook;

// key = order_id
typedef std::unordered_map<uint64_t, OrderInput> OrderInputBook;

// key = order_id
typedef std::unordered_map<uint64_t, Order> OrderBook;

// key = trade_id
typedef std::unordered_map<uint64_t, Trade> TradeBook;

/**
 * @brief Book structure that holds all trading data about all accounts, instruments and exchanges, etc.
 *
 *
 */
struct Book {
    CommissionBook commissions;
    InstrumentBook instruments;
    Asset asset = {};
    AssetMargin asset_margin = {};
    PositionBook positions = {};
    OrderInputBook order_inputs = {};
    OrderBook orders = {};
    TradeBook trades = {};
};

class CommissionBookFn {
public:
    CommissionBookFn(Book &book) : commissions_(book.commissions) {}

    void set(const Commission &commission) { commissions_[infra::hash_str_32(commission.product_id)] = commission; }

    const Commission *get(const infra::Array<char, PRODUCT_ID_LEN> &product_id) const {
        auto it = commissions_.find(infra::hash_str_32(product_id));
        return it != commissions_.end() ? &it->second : nullptr;
    }

private:
    CommissionBook &commissions_;
};

class InstrumentBookFn {
public:
    InstrumentBookFn(Book &book) : instruments_(book.instruments) {}

    void set(const Instrument &instrument) {
        instruments_[hash_instrument(instrument.exchange_id, instrument.instrument_id)] = instrument;
    }

    const Instrument *get(const infra::Array<char, INSTRUMENT_ID_LEN> &instrument_id,
                          const infra::Array<char, EXCHANGE_ID_LEN> &exchange_id) const {
        auto it = instruments_.find(hash_instrument(exchange_id, instrument_id));
        return it != instruments_.end() ? &it->second : nullptr;
    }

private:
    InstrumentBook &instruments_;
};

class PositionBookFn {
public:
    PositionBookFn(PositionBook &positions) : positions_(positions) {}
    explicit PositionBookFn(Book &book) : positions_(book.positions) {}

    void set(const Position &position) {
        auto &positions =
            position.direction == enums::Direction::Long ? positions_.long_positions : positions_.short_positions;
        positions[hash_instrument(position.exchange_id, position.instrument_id)] = position;
    }

    const Position *get(const infra::Array<char, INSTRUMENT_ID_LEN> &instrument_id,
                        const infra::Array<char, EXCHANGE_ID_LEN> &exchange_id, enums::Direction direction) const {
        auto &positions = direction == enums::Direction::Long ? positions_.long_positions : positions_.short_positions;
        auto it = positions.find(hash_instrument(exchange_id, instrument_id));
        return it != positions.end() ? &it->second : nullptr;
    }

private:
    PositionBook &positions_;
};

class OrderInputBookFn {
public:
    OrderInputBookFn(Book &book) : book_(book.order_inputs) {}

    void set(const OrderInput &order_input) { book_[order_input.order_id] = order_input; }

    const OrderInput *get(uint64_t order_id) const {
        auto it = book_.find(order_id);
        return it != book_.end() ? &it->second : nullptr;
    }

private:
    OrderInputBook &book_;
};

class OrderBookFn {
public:
    OrderBookFn(Book &book) : book_(book.orders) {}

    void set(const Order &order) { book_[order.order_id] = order; }

    const Order *get(uint64_t order_id) const {
        auto it = book_.find(order_id);
        return it != book_.end() ? &it->second : nullptr;
    }

private:
    OrderBook &book_;
};

class TradeBookFn {
public:
    TradeBookFn(Book &book) : book_(book.trades) {}

    void set(const Trade &trade) { book_[trade.trade_id] = trade; }

    const Trade *get(uint64_t trade_id) const {
        auto it = book_.find(trade_id);
        return it != book_.end() ? &it->second : nullptr;
    }

private:
    TradeBook &book_;
};

} // namespace btra