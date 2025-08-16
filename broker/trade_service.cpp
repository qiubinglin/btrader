#include "trade_service.h"

#include "broker/brokersim/brokersim.h"
#include "broker/binance/binance_broker.h"

namespace btra::broker {

TradeServiceUPtr TradeService::create(const std::string &institution) {
    if (institution == "binance") {
        return std::make_unique<BinanceBroker>();
    } else if (institution == "simulation") {
        return std::make_unique<BrokerSim>();
    } else {
        throw std::runtime_error("Wrong broker service institution!");
    }
}

} // namespace btra::broker