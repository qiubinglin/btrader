#include "strategy/backtest_executor.h"

namespace btra::strategy {

void BacktestExecutor::add_timer(int64_t time, const CBFunc &callback) {}

void BacktestExecutor::add_time_interval(int64_t duration, const CBFunc &callback) {}

void BacktestExecutor::add_td_account(const std::string &institution, const std::string &account) {}

void BacktestExecutor::subscribe(const std::string &institution, const std::string &account, const MDSubscribe &sub) {}

uint64_t BacktestExecutor::insert_order(const std::string &institution, const std::string &account,
                                        const OrderInput &order) {}

void BacktestExecutor::req_history_order(const std::string &institution, const std::string &account,
                                         const RequestHistoryOrder &req) {}

void BacktestExecutor::req_history_trade(const std::string &institution, const std::string &account,
                                         const RequestHistoryTrade &req) {}

uint64_t BacktestExecutor::cancel_order(uint64_t order_id) {}

int64_t BacktestExecutor::get_trading_day() const {}

void BacktestExecutor::req_deregister() {}

void BacktestExecutor::update_strategy_state(StrategyStateUpdate &state_update) {}

uint64_t BacktestExecutor::req_account_info(const std::string &institution, const std::string &account,
                                            const AccountReq &req) {}

} // namespace btra::strategy