#pragma once

#include <map>
#include <string>

#include "guidb/klineset.h"
#include "guidb/orderbookset.h"
#include "guidb/ticktradebook.h"
#include "infra/common.h"

namespace btra::gui {

/**
 * @brief Database contains all data and provide access interface of these data.
 *
 */
class Database {
public:
    KLineSetSPtr reqKlineDB(const std::string &id);

    OrderBookSetSPtr reqOrderBookSet(const std::string &id);

    TickTradeBookSPtr reqTickTradeBook(const std::string &id);

private:
    /* Klines */
    std::map<std::string, KLineSetSPtr> klinesets_;

    std::map<std::string, OrderBookSetSPtr> orderbooksets_;

    std::map<std::string, TickTradeBookSPtr> ticktradebooks_;
};
DECLARE_SPTR(Database)

} // namespace btra::gui