#pragma once

#include "strategy/executor.h"

namespace btra::strategy {

class LiveExecutor : public Executor {
public:
    explicit LiveExecutor(EventEngine *engine) : Executor(engine) {}

    void add_timer(int64_t time, const CBFunc &callback) override;

    void add_time_interval(int64_t duration, const CBFunc &callback) override;

    void add_td_account(const std::string &institution, const std::string &account) override;

    void subscribe(const std::string &institution, const std::string &account, const MDSubscribe &sub) override;

    uint64_t insert_order(const std::string &institution, const std::string &account, const OrderInput &order) override;

    void req_history_order(const std::string &institution, const std::string &account,
                           const RequestHistoryOrder &req) override;

    void req_history_trade(const std::string &institution, const std::string &account,
                           const RequestHistoryTrade &req) override;

    uint64_t cancel_order(uint64_t order_id) override;

    int64_t get_trading_day() const override;

    void req_deregister() override;

    void update_strategy_state(StrategyStateUpdate &state_update) override;

    uint64_t req_account_info(const std::string &institution, const std::string &account,
                              const AccountReq &req) override;
};

} // namespace btra::strategy