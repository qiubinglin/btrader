#include "broker/backtest/backtest_broker.h"

namespace btra::broker {

void BackTestBroker::setup(const Json::json &cfg) {
    // Implementation for setup
}

void BackTestBroker::start() {
    // Implementation for start
}

enums::AccountType BackTestBroker::get_account_type() const {
    return enums::AccountType::BackTest;
}

bool BackTestBroker::insert_order(const OrderInput &input) {
    // Implementation for insert_order
    return true;
}

bool BackTestBroker::cancel_order(const OrderAction &input) {
    // Implementation for cancel_order
    return true;
}

bool BackTestBroker::req_account_info(const AccountReq &req) {
    // Implementation for req_account_info
    return true;
}

} // namespace btra::broker