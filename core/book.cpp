#include "core/book.h"

#include <iostream>

#include "infra/time.h"

namespace btra {

void Book::update(const Transaction &transaction) { positions.update(transaction); }

void Book::update(const Bar &bar) {
    auto position_ptr = positions.get(bar.instrument_id, bar.exchange_id, enums::Direction::Long);

    if (position_ptr == nullptr) {
        position_ptr = positions.get(bar.instrument_id, bar.exchange_id, enums::Direction::Short);
    }

    if (position_ptr == nullptr) {
        return;
    }

    int factor = position_ptr->direction == enums::Direction::Long ? 1 : -1;
    position_ptr->unrealized_pnl = position_ptr->volume * (bar.close - position_ptr->position_cost_price) * factor;
}

double Book::asset_price() const { return asset.avail + positions.unrealized_pnl(); }

void PositionBook::set(const Position &position) {
    auto &positions = position.direction == enums::Direction::Long ? long_positions : short_positions;
    auto position_ptr = positions[hash_instrument(position.exchange_id, position.instrument_id)] = position;
}

Position *PositionBook::get(const infra::Array<char, INSTRUMENT_ID_LEN> &instrument_id,
                            const infra::Array<char, EXCHANGE_ID_LEN> &exchange_id, enums::Direction direction) {
    auto &positions = direction == enums::Direction::Long ? long_positions : short_positions;
    auto it = positions.find(hash_instrument(exchange_id, instrument_id));
    return it != positions.end() ? &it->second : nullptr;
}

void PositionBook::update(const Transaction &tranction) {
    auto direction = enums::side2direction(tranction.side);
    auto position_ptr = get(tranction.instrument_id, tranction.exchange_id, direction);
    if (position_ptr != nullptr) {
        position_ptr->update_time = infra::time::now_time();
        position_ptr->trading_day = tranction.trading_day;
        position_ptr->position_cost_price =
            (position_ptr->volume * position_ptr->position_cost_price + tranction.volume * tranction.price) /
            (position_ptr->volume += tranction.volume);
    } else {
        Position position;
        position.update_time = infra::time::now_time();
        position.trading_day = tranction.trading_day;
        position.position_cost_price = tranction.price;
        position.volume = tranction.volume;
        position.exchange_id = tranction.exchange_id;
        position.instrument_id = tranction.instrument_id;
        position.instrument_type = tranction.instrument_type;
        set(position);
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