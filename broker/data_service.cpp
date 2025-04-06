#include "data_service.h"

#include "backtest_data.h"

namespace btra::broker {

DataServiceUPtr DataService::create(const std::string &institution) {
    if (institution == "backtest") {
        return std::make_unique<BackTestData>();
    } else {
        throw std::runtime_error("Wrong data service institution!");
    }
}

} // namespace btra::broker