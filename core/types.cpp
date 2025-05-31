#include "types.h"

namespace btra {

MDSubscribe::MDSubscribe(const char *data, uint32_t len) {
    memcpy(&id, data, sizeof(id));
    data += sizeof(id);
    len -= sizeof(id);

    auto keys_count = len / sizeof(InstrumentKey);
    instrument_keys.resize(keys_count);
    char *pos = (char *)instrument_keys.data();
    memcpy(pos, data, sizeof(InstrumentKey) * keys_count);
}

std::string MDSubscribe::to_string() const {
    size_t size = 0;
    size += sizeof(id);
    size += sizeof(InstrumentKey) * instrument_keys.size();

    std::string res(size, 0);
    char *header = (char *)res.data();
    memcpy(header, &id, sizeof(id));
    header += sizeof(id);
    for (const auto &key : instrument_keys) {
        memcpy(header, &key, sizeof(InstrumentKey));
        header += sizeof(InstrumentKey);
    }
    return res;
}

void order_from_input(const OrderInput &input, Order &order) {
    order.order_id = input.order_id;

    order.instrument_id = input.instrument_id;
    order.exchange_id = input.exchange_id;

    order.instrument_type = input.instrument_type;

    order.limit_price = input.limit_price;
    order.frozen_price = input.frozen_price;

    order.volume = input.volume;
    order.volume_left = input.volume;
    order.status = enums::OrderStatus::Pending;

    order.side = input.side;
    order.offset = input.offset;

    order.is_swap = input.is_swap;

    order.hedge_flag = input.hedge_flag;
    order.price_type = input.price_type;
    order.volume_condition = input.volume_condition;
    order.time_condition = input.time_condition;

    order.parent_id = input.parent_id;
}

void trade_from_order(const Order &order, Trade &trade) {
    trade.order_id = order.order_id;
    trade.instrument_id = order.instrument_id;
    trade.exchange_id = order.exchange_id;
    trade.external_order_id = order.external_order_id;
    trade.instrument_type = order.instrument_type;
    trade.side = order.side;
    trade.offset = order.offset;
    trade.hedge_flag = order.hedge_flag;
}

} // namespace btra