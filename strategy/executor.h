#pragma once

#include "eventengine.h"
#include "infra/infra.h"
#include "types.h"

namespace btra::strategy {

class Executor : public std::enable_shared_from_this<Executor> {
public:
    Executor(EventEngine *engine);

    void set_engine(EventEngine *engine);

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
    virtual void add_timer(int64_t time, const CBFunc &callback);

    /**
     * @brief Add periodically callback.
     * @param duration duration in seconds or nano seconds
     * @param callback callback function
     */
    virtual void add_time_interval(int64_t duration, const CBFunc &callback);

    /**
     * @brief Add td account for strategy.
     * @param institution The institution which the account belongs to
     * @param account TD account ID
     */
    virtual void add_td_account(const std::string &institution, const std::string &account);

    /**
     * @brief Subscribe market data.
     * @param institution The institution which the account belongs to
     * @param account MD account ID
     * @param sub data to be subscribed
     */
    virtual void subscribe(const std::string &institution, const std::string &account, const MDSubscribe &sub);

    /**
     * @brief Insert order
     * @param institution The institution which the account belongs to
     * @param account account ID
     * @param order order
     * @return order id
     */
    virtual uint64_t insert_order(const std::string &institution, const std::string &account, const OrderInput &order);

    /**
     * @brief query history order
     * @param institution The institution which the account belongs to
     * @param account account ID
     */
    virtual void req_history_order(const std::string &institution, const std::string &account,
                                   const RequestHistoryOrder &req);

    /**
     * @brief query history trade
     */
    virtual void req_history_trade(const std::string &institution, const std::string &account,
                                   const RequestHistoryTrade &req);

    /**
     * @brief Cancel order.
     * @param order_id order ID
     * @return order action ID
     */
    virtual uint64_t cancel_order(uint64_t order_id);

    /**
     * @brief Get current trading day.
     * @return current trading day
     */
    virtual int64_t get_trading_day() const;

    /**
     * @brief Tells whether the book is held.
     * If book is held, all traded positions will be recovered from ledger.
     * If book is not held, ledger will use the information collected in pre_start to build a fresh book.
     * @return true if book is held, false otherwise. Defaults to false.
     */
    [[nodiscard]] bool is_book_held() const;

    /**
     * @brief Tells whether to mirror positions from enrolled accounts.
     * If positions are mirrored, will always have positions equal to the corresponding positions under accounts.
     * If positions are not mirrored, will set positions to 0 for newly added strategies, or the history values recorded
     * by kungfu.
     * @return true if positions are mirrored, false otherwise. Defaults to true.
     */
    [[nodiscard]] bool is_positions_mirrored() const;

    /**
     * @brief Call to hold book.
     */
    void hold_book();

    /**
     * @brief Call to hold positions, i.e. do not mirror positions.
     */
    void hold_positions();

    /**
     * @brief Call to skip bookkeeper calculation
     */
    void bypass_accounting();

    /**
     * @brief Tells whether skip bookkeeper
     * @return true to skip bookkeeper, false to using bookkeeper
     */
    bool is_bypass_accounting() const;

    /**
     * @brief request deregister.
     * @return void
     */
    virtual void req_deregister(){};

    /**
     * @brief Update Strategy State
     * @param state StrategyState
     * @param infos vector<string>, info_a, info_b, info_c.
     */
    virtual void update_strategy_state(StrategyStateUpdate &state_update){};

    /**
     * @brief Get arguments kfc run -a
     * @return string of arguments
     */
    virtual std::string arguments() { return {}; };

private:
    bool book_held_ = false;
    bool positions_mirrored_ = true;
    bool bypass_accounting_ = false;

    EventEngine *engine_ = nullptr;
};
DECLARE_SPTR(Executor)

} // namespace btra::strategy