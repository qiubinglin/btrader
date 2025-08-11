#pragma once

#include "core/hashid.h"
#include "core/types.h"

namespace btra {

// key = hash_str_32(product_id) or key = hash_product(exchange_id, product_id)
class CommissionBook : public std::unordered_map<uint32_t, Commission> {
public:
    void set(const Commission &commission) { at(infra::hash_str_32(commission.product_id)) = commission; }

    const Commission *get(const infra::Array<char, PRODUCT_ID_LEN> &product_id) const {
        auto it = find(infra::hash_str_32(product_id));
        return it != end() ? &it->second : nullptr;
    }
};

// key = hash_instrument(exchange_id, instrument_id)
class InstrumentBook : public std::unordered_map<uint32_t, Instrument> {
public:
    void set(const Instrument &instrument) {
        at(hash_instrument(instrument.exchange_id, instrument.instrument_id)) = instrument;
    }

    const Instrument *get(const infra::Array<char, INSTRUMENT_ID_LEN> &instrument_id,
                          const infra::Array<char, EXCHANGE_ID_LEN> &exchange_id) const {
        auto it = find(hash_instrument(exchange_id, instrument_id));
        return it != end() ? &it->second : nullptr;
    }
};

// key = order_id
class OrderInputBook : public std::unordered_map<uint64_t, OrderInput> {
public:
    void set(const OrderInput &order_input) { at(order_input.order_id) = order_input; }

    const OrderInput *get(uint64_t order_id) const {
        auto it = find(order_id);
        return it != end() ? &it->second : nullptr;
    }
};

// key = order_id
class OrderBook : public std::unordered_map<uint64_t, Order> {
public:
    void set(const Order &order) { at(order.order_id) = order; }

    const Order *get(uint64_t order_id) const {
        auto it = find(order_id);
        return it != end() ? &it->second : nullptr;
    }
};

// key = trade_id
class TradeBook : public std::unordered_map<uint64_t, Trade> {
public:
    void set(const Trade &trade) { at(trade.trade_id) = trade; }

    const Trade *get(uint64_t trade_id) const {
        auto it = find(trade_id);
        return it != end() ? &it->second : nullptr;
    }
};

/**
 * @brief Book structure that holds all trading data about all accounts, instruments and exchanges, etc.
 * All *Fn will be removed.
 *
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

    void update(const Transaction &transaction);
    void update(const Bar &bar);
    double asset_price() const;
};

} // namespace btra