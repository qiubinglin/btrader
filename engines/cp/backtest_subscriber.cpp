#include "backtest_subscriber.h"

namespace btra {

void BacktestSubscriber::pre_start() {}
void BacktestSubscriber::post_stop() {}
void BacktestSubscriber::on_trading_day(const EventSPtr &event) {}
void BacktestSubscriber::on_bar(const EventSPtr &event) {}
void BacktestSubscriber::on_quote(const EventSPtr &event) {}
void BacktestSubscriber::on_tree(const EventSPtr &event) {}
void BacktestSubscriber::on_entrust(const EventSPtr &event) {}
void BacktestSubscriber::on_transaction(const EventSPtr &event) {}
void BacktestSubscriber::on_order_action_error(const EventSPtr &event) {}
void BacktestSubscriber::on_trade(const EventSPtr &event) {}
void BacktestSubscriber::on_position_sync_reset(const EventSPtr &event) {}
void BacktestSubscriber::on_asset_sync_reset(const EventSPtr &event) {}
void BacktestSubscriber::on_asset_margin_sync_reset(const EventSPtr &event) {}
void BacktestSubscriber::on_deregister(const EventSPtr &event) {}
void BacktestSubscriber::on_broker_state_change(const EventSPtr &event) {}
void BacktestSubscriber::on_custom_data(const EventSPtr &event) {}

} // namespace btra