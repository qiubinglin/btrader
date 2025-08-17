#include "strategy/live_executor.h"

namespace btra::strategy {

void LiveExecutor::add_timer(int64_t time, const CBFunc &callback) { ExtScheduler(*engine_).add_timer(time, callback); }

void LiveExecutor::add_time_interval(int64_t duration, const CBFunc &callback) {
    ExtScheduler(*engine_).add_time_interval(duration, callback);
}

void LiveExecutor::add_td_account(const std::string &institution, const std::string &account) {}

void LiveExecutor::subscribe(const std::string &institution, const std::string &account, const MDSubscribe &sub) {}

uint64_t LiveExecutor::insert_order(const std::string &institution, const std::string &account,
                                    const OrderInput &order) {
    auto account_location_uid = journal::JIDUtil::build(institution, account);
    auto writer = engine_->get_writer(account_location_uid);

    OrderInput &input = writer->open_data<OrderInput>(now_event_time());
    input = order;

    input.order_id = writer->current_frame_uid();
    input.insert_time = infra::time::now_time();

    writer->close_data();
    return input.order_id;
}

void LiveExecutor::req_history_order(const std::string &institution, const std::string &account,
                                     const RequestHistoryOrder &req) {}

void LiveExecutor::req_history_trade(const std::string &institution, const std::string &account,
                                     const RequestHistoryTrade &req) {}

uint64_t LiveExecutor::cancel_order(uint64_t order_id) {
    auto td_uid = engine_->get_main_cfg().get_td_location_uid();
    uint32_t account_location_uid = (order_id >> 32u) xor td_uid;
    auto writer = engine_->get_writer(account_location_uid);

    OrderCancel &action = writer->open_data<OrderCancel>(now_event_time());
    action.order_id = writer->current_frame_uid();
    action.target_order_id = order_id;

    uint64_t order_action_id = action.order_id;
    writer->close_data();
    return order_action_id;
}

int64_t LiveExecutor::get_trading_day() const { return 0; }

void LiveExecutor::req_deregister() {}

void LiveExecutor::update_strategy_state(StrategyStateUpdate &state_update) {}

uint64_t LiveExecutor::req_account_info(const std::string &institution, const std::string &account,
                                        const AccountReq &req) {
    auto account_location_uid = journal::JIDUtil::build(institution, account);
    auto writer = engine_->get_writer(account_location_uid);

    AccountReq &account_req = writer->open_data<AccountReq>(now_event_time());
    account_req = req;

    account_req.id = writer->current_frame_uid();
    account_req.insert_time = infra::time::now_time();
    writer->close_data();

    return account_req.id;
}

} // namespace btra::strategy