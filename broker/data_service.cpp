#include "data_service.h"

#include "backtest_data.h"
#include "broker/binance/binance_data.h"

namespace btra::broker {

DataServiceUPtr DataService::create(const std::string &institution) {
    if (institution == "backtest") {
        return std::make_unique<BackTestData>();
    } else if (institution == "binance") {
        return std::make_unique<BinanceData>();
    } else {
        throw std::runtime_error("Wrong data service institution!");
    }
}

} // namespace btra::broker