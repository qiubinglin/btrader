#include "data_service.h"

#include "broker/filedataservice/filedataservice.h"
#include "broker/binance/binance_data.h"

namespace btra::broker {

DataServiceUPtr DataService::create(const std::string &institution) {
    if (institution == "binance") {
        return std::make_unique<BinanceData>();
    } else if (institution == "csv") {
        return std::make_unique<FileDataService>();
    } else {
        throw std::runtime_error("Wrong data service institution!");
    }
}

} // namespace btra::broker