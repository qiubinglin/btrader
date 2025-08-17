#include "broker/pyservice/pytrade_service.h"

namespace btra::broker {

void PyTradeService::setup(const Json::json &cfg) {
    // Implementation for setup
}

void PyTradeService::start() {
    // Implementation for start
}

void PyTradeService::stop() {
    // Implementation for stop
}

enums::AccountType PyTradeService::get_account_type() const {
    return enums::AccountType::BackTest;
}

bool PyTradeService::insert_order(const OrderInput &input) {
    // Implementation for insert_order
    return true;
}

bool PyTradeService::cancel_order(const OrderCancel &input) {
    // Implementation for cancel_order
    return true;
}

bool PyTradeService::req_account_info(const AccountReq &req) {
    // Implementation for req_account_info
    return true;
}

} // namespace btra::broker