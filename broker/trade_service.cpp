#include "trade_service.h"

#include "broker/binance/binance_broker.h"

namespace btra::broker {

TradeServiceUPtr TradeService::create(const std::string &institution) {
    if (institution == "backtest") {
        return nullptr;
    } else if (institution == "binance") {
        return std::make_unique<BinanceBroker>();
    } else {
        throw std::runtime_error("Wrong broker service institution!");
    }
}

} // namespace btra::broker