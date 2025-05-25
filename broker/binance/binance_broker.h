#pragma once

#include "broker/trade_service.h"
#include "infra/websocket_client.h"
#include "infra/crypto/signer.h"

namespace btra::broker {

class BinanceBroker : public TradeService {
public:
    ~BinanceBroker();
    void setup(const Json::json &cfg);
    void start();
    enums::AccountType get_account_type() const;

    bool insert_order(const OrderInput &input);
    bool cancel_order(const OrderAction &input);

private:
    void on_msg(const std::string &msg);

    std::string public_key_str_;
    infra::crypto::EncrypKey public_key_;
    infra::crypto::EncrypKey secret_key_;

    infra::WebSocketClient client_;
    std::string uri_;
};

} // namespace btra::broker