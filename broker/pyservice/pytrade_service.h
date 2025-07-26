#pragma once

#include <pybind11/embed.h>

#include "broker/trade_service.h"

namespace btra::broker {

class __attribute__((visibility("default"))) PyTradeService : public TradeService {
public:
    void setup(const Json::json &cfg) override;
    void start() override;
    void stop() override;
    enums::AccountType get_account_type() const override;
    bool insert_order(const OrderInput &input) override;
    bool cancel_order(const OrderAction &input) override;
    bool req_account_info(const AccountReq &req) override;

private:
    // pybind11::object impl_;
};

} // namespace btra::broker