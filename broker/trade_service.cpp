#include "trade_service.h"

#include "broker/backtest/backtest_broker.h"
#include "broker/binance/binance_broker.h"

namespace btra::broker {

TradeServiceUPtr TradeService::create(const std::string &institution, enums::BacktestDataType backtest_data_type) {
    if (backtest_data_type == enums::BacktestDataType::None) {
        if (institution == "binance") {
            return std::make_unique<BinanceBroker>();
        } else {
            throw std::runtime_error("Wrong broker service institution!");
        }
    }

    return std::make_unique<BackTestBroker>();
}

} // namespace btra::broker