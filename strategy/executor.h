#pragma once

#include "core/book.h"
#include "core/enums.h"
#include "eventengine.h"
#include "infra/infra.h"
#include "types.h"

namespace btra::strategy {

FORWARD_DECLARE_SPTR(Executor)

class Executor : public std::enable_shared_from_this<Executor> {
public:
    static ExecutorSPtr create(enums::RunMode mode, EventEngine *engine);

    explicit Executor(EventEngine *engine);
    virtual ~Executor() = default;

    void set_engine(EventEngine *engine);

    const Book &book() const;
    Book &book();

    /**
     * @brief Get current time in seconds or nano seconds
     * @return current time in seconds or nano seconds
     */
    virtual int64_t now_event_time() const;

    /**
     * @brief Add one shot timer callback.
     * @param time when to call in seconds or nano seconds
     * @param callback callback function
     */
    virtual void add_timer(int64_t time, const CBFunc &callback) = 0;

    /**
     * @brief Add periodically callback.
     * @param duration duration in seconds or nano seconds
     * @param callback callback function
     */
    virtual void add_time_interval(int64_t duration, const CBFunc &callback) = 0;

    /**
     * @brief Add td account for strategy.
     * @param institution The institution which the account belongs to
     * @param account TD account ID
     */
    virtual void add_td_account(const std::string &institution, const std::string &account) = 0;

    /**
     * @brief Subscribe market data.
     * @param institution The institution which the account belongs to
     * @param account MD account ID
     * @param sub data to be subscribed
     */
    virtual void subscribe(const std::string &institution, const std::string &account, const MDSubscribe &sub) = 0;

    /**
     * @brief Insert order
     * @param institution The institution which the account belongs to
     * @param account account ID
     * @param order order
     * @return order id
     */
    virtual uint64_t insert_order(const std::string &institution, const std::string &account,
                                  const OrderInput &order) = 0;

    /**
     * @brief query history order
     * @param institution The institution which the account belongs to
     * @param account account ID
     */
    virtual void req_history_order(const std::string &institution, const std::string &account,
                                   const RequestHistoryOrder &req) = 0;

    /**
     * @brief query history trade
     */
    virtual void req_history_trade(const std::string &institution, const std::string &account,
                                   const RequestHistoryTrade &req) = 0;

    /**
     * @brief Cancel order.
     * @param order_id order ID
     * @return order action ID
     */
    virtual uint64_t cancel_order(uint64_t order_id) = 0;

    /**
     * @brief Get current trading day.
     * @return current trading day
     */
    virtual int64_t get_trading_day() const = 0;

    /**
     * @brief request deregister.
     * @return void
     */
    virtual void req_deregister() = 0;

    /**
     * @brief Update Strategy State
     * @param state StrategyState
     * @param infos vector<string>, info_a, info_b, info_c.
     */
    virtual void update_strategy_state(StrategyStateUpdate &state_update) = 0;

    /**
     * @brief Request account information.
     *
     * @param institution The institution which the account belongs to
     * @param account The account ID
     * @param req The account request
     * @return uint64_t The request ID
     */
    virtual uint64_t req_account_info(const std::string &institution, const std::string &account,
                                      const AccountReq &req) = 0;

protected:
    Book book_;

    EventEngine *engine_ = nullptr;
};

} // namespace btra::strategy