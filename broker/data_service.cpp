#include "data_service.h"

#include "broker/backtest/backtest_csvdata.h"
#include "broker/binance/binance_data.h"

namespace btra::broker {

DataServiceUPtr DataService::create(const std::string &institution, enums::BacktestDataType backtest_data_type) {
    if (backtest_data_type == enums::BacktestDataType::None) {
        if (institution == "binance") {
            return std::make_unique<BinanceData>();
        } else {
            throw std::runtime_error("Wrong data service institution!");
        }
    }

    switch (backtest_data_type) {
        case enums::BacktestDataType::CSV:
            return std::make_unique<BackTestCSVData>();
            break;
        default:
            throw std::runtime_error("Wrong backtest_data_type!");
            break;
    }
}

} // namespace btra::broker