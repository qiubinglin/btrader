#include "database.h"

#include <memory>
#include "guidb/ticktradebook.h"

namespace btra::gui {

KLineSetSPtr Database::reqKlineDB(const std::string &id) {
    if (!klinesets_.contains(id)) {
        klinesets_[id] = std::make_shared<KLineSet>();
    }
    return klinesets_[id];
}

OrderBookSetSPtr Database::reqOrderBookSet(const std::string &id) {
    if (!orderbooksets_.contains(id)) {
        orderbooksets_[id] = std::make_shared<OrderBookSet>();
    }
    return orderbooksets_[id];
}

TickTradeBookSPtr Database::reqTickTradeBook(const std::string &id) {
    if (!ticktradebooks_.contains(id)) {
        ticktradebooks_[id] = std::make_shared<TickTradeBook>();
    }
    return ticktradebooks_[id];
}

} // namespace btra::gui