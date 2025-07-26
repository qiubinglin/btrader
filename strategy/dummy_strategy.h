#pragma once

#include "core/book.h"
#include "core/types.h"
#include "strategy/strategy.h"

namespace btra::strategy {

class DummyStrategy : public Strategy {
public:
    DummyStrategy() { printf("DummyStrategy\n"); }

    void on_quote(ExecutorSPtr &executor, const Quote &quote, JID source) override {}

    void on_bar(ExecutorSPtr &executor, const Bar &bar, JID source) override {}

    void on_order(ExecutorSPtr &executor, const Order &order, JID source) override {}

    void on_order_action_error(ExecutorSPtr &executor, const OrderActionError &error, JID source) override {}

    void on_position_sync_reset(ExecutorSPtr &executor, const PositionBook &old_book, const PositionBook &new_book,
                                JID source) override {}

    void on_asset_sync_reset(ExecutorSPtr &executor, const Asset &old_asset, const Asset &new_asset,
                             JID source) override {}

    void on_broker_state_change(ExecutorSPtr &executor, const BrokerStateUpdate &broker_state_update,
                                JID source) override {}
};

} // namespace btra::strategy