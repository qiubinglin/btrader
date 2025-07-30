#pragma once

#include "broker/trade_service.h"

namespace btra::broker {

class BrokerSim : public TradeService {
public:
    void setup(const Json::json &cfg) override;
    void start() override;
    void stop() override;
    enums::AccountType get_account_type() const override;
    bool insert_order(const OrderInput &input) override;
    bool cancel_order(const OrderAction &input) override;
    bool req_account_info(const AccountReq &req) override;

private:
    double commission_rate{0};
};

} // namespace btra::broker
