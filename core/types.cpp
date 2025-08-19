#include "types.h"

namespace btra {

Deregister::Deregister(const char *data, [[maybe_unused]] uint32_t len) {
    memcpy(&location_uid, data, sizeof(location_uid));
    data += sizeof(location_uid);
    len -= sizeof(location_uid);

    memcpy(&category, data, sizeof(category));
    data += sizeof(category);
    len -= sizeof(category);

    memcpy(&mode, data, sizeof(mode));
    data += sizeof(mode);
    len -= sizeof(mode);

    auto group_len = *reinterpret_cast<const uint32_t *>(data);
    data += sizeof(uint32_t);
    len -= sizeof(uint32_t);
    group.assign(data, group_len);
    data += group_len;
    len -= group_len;

    auto name_len = *reinterpret_cast<const uint32_t *>(data);
    data += sizeof(uint32_t);
    len -= sizeof(uint32_t);
    name.assign(data, name_len);
}

std::string Deregister::to_string() const {
    size_t size = sizeof(location_uid) + sizeof(category) + sizeof(mode);
    size += sizeof(uint32_t) + group.size();
    size += sizeof(uint32_t) + name.size();

    std::string res(size, 0);
    char *header = (char *)res.data();
    memcpy(header, &location_uid, sizeof(location_uid));
    header += sizeof(location_uid);
    memcpy(header, &category, sizeof(category));
    header += sizeof(category);
    memcpy(header, &mode, sizeof(mode));
    header += sizeof(mode);

    *reinterpret_cast<uint32_t *>(header) = group.size();
    header += sizeof(uint32_t);
    memcpy(header, group.data(), group.size());
    header += group.size();

    *reinterpret_cast<uint32_t *>(header) = name.size();
    header += sizeof(uint32_t);
    memcpy(header, name.data(), name.size());

    return res;
}

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

void trade_from_orderinput(const OrderInput &input, Trade &trade) {
    // Generate a unique trade ID (this should be provided by the exchange in real scenarios)
    // For now, we use the order_id as a placeholder trade_id
    trade.trade_id = input.order_id;
    
    // Copy order information
    trade.order_id = input.order_id;
    
    // Set external IDs (these would be filled by the exchange in real trading)
    // trade.external_order_id and trade.external_trade_id are left empty for now
    // In a real implementation, these would be provided by the broker/exchange
    
    // Set timing information
    trade.trade_time = input.insert_time;
    // Note: trading_day would need to be set based on the actual trading day
    // This could be extracted from the current system time or trading calendar
    
    // Copy instrument information
    trade.instrument_id = input.instrument_id;
    trade.exchange_id = input.exchange_id;
    trade.instrument_type = input.instrument_type;
    
    // Copy trading parameters
    trade.side = input.side;
    trade.offset = input.offset;
    trade.hedge_flag = input.hedge_flag;
    
    // Set price and volume
    trade.price = input.limit_price;
    trade.volume = input.volume;
    
    // Initialize fees (these would be calculated by the exchange in real scenarios)
    // Tax and commission are typically calculated based on:
    // - Exchange rules
    // - Broker fees
    // - Regulatory requirements
    // - Trade volume and price
    trade.tax = 0.0;
    trade.commission = 0.0;
}

} // namespace btra