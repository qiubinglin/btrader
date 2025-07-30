#include "brokersim.h"

namespace btra::broker {

void BrokerSim::setup(const Json::json &cfg) {
    // Implementation for setup
}

void BrokerSim::start() {
    // Implementation for start
}

void BrokerSim::stop() {
    // Implementation for stop
}

enums::AccountType BrokerSim::get_account_type() const {
    return enums::AccountType::BackTest;
}

bool BrokerSim::insert_order(const OrderInput &input) {
    // Implementation for insert_order
    return true;
}

bool BrokerSim::cancel_order(const OrderAction &input) {
    // Implementation for cancel_order
    return true;
}

bool BrokerSim::req_account_info(const AccountReq &req) {
    // Implementation for req_account_info
    return true;
}

}