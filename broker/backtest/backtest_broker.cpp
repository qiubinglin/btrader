#include "broker/backtest/backtest_broker.h"

namespace btra::broker {

void BackTestBroker::setup(const Json::json &) {
    // Implementation for setup
}

void BackTestBroker::start() {
    // Implementation for start
}

void BackTestBroker::stop() {
    // Implementation for stop
}

enums::AccountType BackTestBroker::get_account_type() const {
    return enums::AccountType::BackTest;
}

bool BackTestBroker::insert_order(const OrderInput &) {
    // Implementation for insert_order
    return true;
}

bool BackTestBroker::cancel_order(const OrderAction &) {
    // Implementation for cancel_order
    return true;
}

bool BackTestBroker::req_account_info(const AccountReq &) {
    // Implementation for req_account_info
    return true;
}

} // namespace btra::broker