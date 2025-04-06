#pragma once

#include <memory>

#include "types.h"

namespace btra::broker {

FORWARD_DECLARE_UPTR(TradeService)
class TradeService {
public:
    static TradeServiceUPtr create(const std::string &institution);
    virtual void setup(const Json::json &cfg) = 0;
    virtual void start() = 0;
    [[nodiscard]] virtual enums::AccountType get_account_type() const = 0;

    virtual bool insert_order(const OrderInput &input) = 0;
    virtual bool cancel_order(const OrderAction &input) = 0;
};

} // namespace btra::broker