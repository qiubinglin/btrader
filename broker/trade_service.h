#pragma once

#include <memory>

#include "book.h"
#include "core/journal/writer.h"
#include "core/types.h"
#include "infra/time.h"
#include "jid.h"

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
     * @brief Stop the trade service.
     *
     */
    virtual void stop() = 0;

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
    virtual bool cancel_order(const OrderCancel &input) = 0;

    virtual bool req_account_info(const AccountReq &req) = 0;

    /**
     * @brief Notice cp about action response
     *
     * @tparam T
     * @param response
     * @return true
     * @return false
     */
    template <typename T>
    bool notify_response(const T &response) {
        auto id = journal::JIDUtil::build(journal::JIDUtil::TD_RESPONSE);
        if (writers_ == nullptr) {
            return false;
        }
        writers_->at(id)->write(infra::time::now_time(), response);
        return true;
    }

    void set_writers(WriterMap *writers) { writers_ = writers; }

protected:
    enums::BrokerState state_ = enums::BrokerState::DisConnected;
    WriterMap *writers_{nullptr};
};

} // namespace btra::broker