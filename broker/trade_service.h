#pragma once

#include <memory>

#include "core/types.h"

namespace btra::broker {

FORWARD_DECLARE_UPTR(TradeService)
/**
 * @brief Base class to provide trade service.
 *
 */
class TradeService {
public:
    /**
     * @brief Factory function to create different trade service according to institution.
     *
     * @param institution
     * @return TradeServiceUPtr
     */
    static TradeServiceUPtr create(const std::string &institution);

    virtual ~TradeService() = default;

    /**
     * @brief Setup config.
     *
     * @param cfg
     */
    virtual void setup(const Json::json &cfg) = 0;

    /**
     * @brief Start the trade service.
     *
     */
    virtual void start() = 0;

    /**
     * @brief Get the state of broker.
     *
     * @return enums::BrokerState
     */
    virtual enums::BrokerState get_state() const { return state_; }

    /**
     * @brief Get the account type object
     *
     * @return enums::AccountType
     */
    [[nodiscard]] virtual enums::AccountType get_account_type() const = 0;

    /**
     * @brief Insert order.
     *
     * @param input
     * @return true
     * @return false
     */
    virtual bool insert_order(const OrderInput &input) = 0;

    /**
     * @brief Cancel order.
     *
     * @param input
     * @return true
     * @return false
     */
    virtual bool cancel_order(const OrderAction &input) = 0;

    virtual bool req_account_info(const AccountReq &req) = 0;

protected:
    enums::BrokerState state_ = enums::BrokerState::DisConnected;
};

} // namespace btra::broker