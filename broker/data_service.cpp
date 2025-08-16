#include "data_service.h"

#include "broker/backtest/backtest_csvdata.h"
#include "broker/binance/binance_data.h"

namespace btra::broker {

DataServiceUPtr DataService::create(const std::string &institution) {
    if (institution == "binance") {
        return std::make_unique<BinanceData>();
    } else if (institution == "csv") {
        return std::make_unique<BackTestCSVData>();
    } else {
        throw std::runtime_error("Wrong data service institution!");
    }
}

} // namespace btra::broker