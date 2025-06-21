#pragma once

#include "cp/cp_engine.h"

namespace btra {

class BacktestSubscriber {
public:
    BacktestSubscriber(CPEngine *engine) : engine_(engine) {}

    void pre_start();
    void post_stop();
    void on_trading_day(const EventSPtr &event);
    void on_bar(const EventSPtr &event);
    void on_quote(const EventSPtr &event);
    void on_tree(const EventSPtr &event);
    void on_entrust(const EventSPtr &event);
    void on_transaction(const EventSPtr &event);
    void on_order_action_error(const EventSPtr &event);
    void on_trade(const EventSPtr &event);
    void on_position_sync_reset(const EventSPtr &event);
    void on_asset_sync_reset(const EventSPtr &event);
    void on_asset_margin_sync_reset(const EventSPtr &event);
    void on_deregister(const EventSPtr &event);
    void on_broker_state_change(const EventSPtr &event);
    void on_custom_data(const EventSPtr &event);

private:
    CPEngine *engine_; /* Make public for invoke. */

    friend class Invoker;
};

} // namespace btra