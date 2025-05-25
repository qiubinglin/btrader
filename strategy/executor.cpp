#include "executor.h"

namespace btra::strategy {

Executor::Executor(EventEngine *engine) { set_engine(engine); }

void Executor::set_engine(EventEngine *engine) { engine_ = engine; }

int64_t Executor::now_event_time() const { return engine_->now_event_time(); }

void Executor::add_timer(int64_t time, const CBFunc &callback) { engine_->ext_scheduler().add_timer(time, callback); }

void Executor::add_time_interval(int64_t duration, const CBFunc &callback) {
    engine_->ext_scheduler().add_time_interval(duration, callback);
}

void Executor::add_td_account(const std::string &institution, const std::string &account) {}

void Executor::subscribe(const std::string &institution, const std::string &account, const MDSubscribe &sub) {}

uint64_t Executor::insert_order(const std::string &institution, const std::string &account, const OrderInput &order) {
    auto account_location_uid = journal::JIDUtil::build(institution, account);
    auto writer = engine_->get_writer(account_location_uid);

    OrderInput &input = writer->open_data<OrderInput>(now_event_time());
    input = order;

    input.order_id = writer->current_frame_uid();
    input.insert_time = infra::time::now_time();

    writer->close_data();
    return input.order_id;
}

void Executor::req_history_order(const std::string &institution, const std::string &account,
                                 const RequestHistoryOrder &req) {}

void Executor::req_history_trade(const std::string &institution, const std::string &account,
                                 const RequestHistoryTrade &req) {}

uint64_t Executor::cancel_order(uint64_t order_id) {
    auto td_uid = engine_->get_main_cfg().get_td_location_uid();
    uint32_t account_location_uid = (order_id >> 32u) xor td_uid;
    auto writer = engine_->get_writer(account_location_uid);

    OrderAction &action = writer->open_data<OrderAction>(now_event_time());
    action.order_action_id = writer->current_frame_uid();
    action.order_id = order_id;
    action.action_flag = enums::OrderActionFlag::Cancel;

    uint64_t order_action_id = action.order_action_id;
    writer->close_data();
    return order_action_id;
}

int64_t Executor::get_trading_day() const { return 0; }

bool Executor::is_book_held() const { return book_held_; }

bool Executor::is_positions_mirrored() const { return positions_mirrored_; }

void Executor::hold_book() { book_held_ = true; }

void Executor::hold_positions() { positions_mirrored_ = false; }

void Executor::bypass_accounting() { bypass_accounting_ = true; }

bool Executor::is_bypass_accounting() const { return bypass_accounting_; }

} // namespace btra::strategy