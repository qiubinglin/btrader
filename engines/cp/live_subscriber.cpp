#include "live_subscriber.h"

#include "extension/globalparams.h"
#include "infra/singleton.h"
#include "infra/time.h"
#include "jid.h"
#include "strategy_invoke.h"
#include "types.h"

namespace btra {

void LiveSubscriber::pre_start() {
    // INFRA_LOG_DEBUG("pre_start");
    TradingStart trading_start;
    trading_start.sync_time = engine_->begin_time_;
    auto req_md_dest = journal::JIDUtil::build(journal::JIDUtil::MD_REQ);
    auto now_time = infra::time::now_time();
    engine_->writers_[req_md_dest]->write(now_time, trading_start);

    for (auto &[key, writer] : engine_->writers_) {
        if (key == req_md_dest) {
            continue;
        }
        writer->write(now_time, trading_start);
    }

    Invoker::invoke(*this, &strategy::Strategy::pre_start);

    if (INSTANCE(GlobalParams).is_backtest) {
        BacktestSyncSignal signal;
        signal.flag = BacktestSyncSignal::MarketData;
        engine_->writers_[req_md_dest]->write(infra::time::now_time(), signal);
    }
}

void LiveSubscriber::post_stop() {}

void LiveSubscriber::on_trading_day(const EventSPtr &event) {
    if (engine_->trading_msg_count_++ != 0) {
        engine_->trading_msg_count_ %= engine_->md_account_count_;
        return;
    }
    const auto &trading_day = event->data<TradingDay>();
    auto daytime = trading_day.timestamp;
    Invoker::invoke(*this, &strategy::Strategy::on_trading_day, daytime);
}

void LiveSubscriber::on_bar(const EventSPtr &event) {
    const auto &bar = event->data<Bar>();
    /* Update executor book with new bar. */
    engine_->executor_->book().update(bar);

    Invoker::invoke(*this, &strategy::Strategy::on_bar, bar, event->source());
    if (INSTANCE(GlobalParams).stat_params.stat_all()) {
        /* Compute floating profit indicators according to bar and current positions? */
        auto &statistics_dump = engine_->statistics_dump_;
        statistics_dump.log_asset(bar.end_time, engine_->executor_->book().asset_price());

        statistics_dump.log_kline(bar);
    }
}

void LiveSubscriber::on_quote(const EventSPtr &event) {
    if (INSTANCE(GlobalParams).is_simulation) {
        const auto &data = event->data<Quote>();
        InstrumentDepth<20> input;
        input.real_depth_size = 20;
        input.instrument_id = data.instrument_id;
        input.exchange_id = data.exchange_id;
        input.ask_price = data.ask_price;
        input.bid_price = data.bid_price;
        input.ask_volume = data.ask_volume;
        input.bid_volume = data.bid_volume;
        engine_->simulation_depth_callboard_->set(input.instrument_id, input);
    }
    Invoker::invoke(*this, &strategy::Strategy::on_quote, event->data<Quote>(), event->source());
}

void LiveSubscriber::on_entrust(const EventSPtr &event) {
    Invoker::invoke(*this, &strategy::Strategy::on_entrust, event->data<Entrust>(), event->source());
}

void LiveSubscriber::on_transaction(const EventSPtr &event) {
    Invoker::invoke(*this, &strategy::Strategy::on_transaction, event->data<Transaction>(), event->source());
}

void LiveSubscriber::on_order_action_error(const EventSPtr &event) {
    Invoker::invoke(*this, &strategy::Strategy::on_order_action_error, event->data<OrderActionResp>(), event->source());
}

void LiveSubscriber::on_trade(const EventSPtr &event) {
    engine_->executor_->book().update(event->data<Trade>());
    if (INSTANCE(GlobalParams).stat_params.stat_all()) {
        engine_->statistics_dump_.log_trade(event->data<Trade>());
    }
    Invoker::invoke(*this, &strategy::Strategy::on_trade, event->data<Trade>(), event->source());
}

void LiveSubscriber::on_position_sync_reset(const EventSPtr &event) {
    PositionBook new_position_book = event->data<PositionBook>();
    Invoker::invoke(*this, &strategy::Strategy::on_position_sync_reset, engine_->executor_->book().positions,
                    new_position_book, event->source());
    engine_->executor_->book().positions = new_position_book;
}

void LiveSubscriber::on_asset_sync_reset(const EventSPtr &event) {
    Invoker::invoke(*this, &strategy::Strategy::on_asset_sync_reset, engine_->executor_->book().asset,
                    event->data<Asset>(), event->source());
    engine_->executor_->book().asset = event->data<Asset>();
}

void LiveSubscriber::on_asset_margin_sync_reset(const EventSPtr &event) {
    Invoker::invoke(*this, &strategy::Strategy::on_asset_margin_sync_reset, engine_->executor_->book().asset_margin,
                    event->data<AssetMargin>(), event->source());
    engine_->executor_->book().asset_margin = event->data<AssetMargin>();
}

void LiveSubscriber::on_deregister(const EventSPtr &event) {
    Invoker::invoke(*this, &strategy::Strategy::on_deregister, event->data<Deregister>(), event->source());
}

void LiveSubscriber::on_broker_state_change(const EventSPtr &event) {
    Invoker::invoke(*this, &strategy::Strategy::on_broker_state_change, event->data<BrokerStateUpdate>(),
                    event->source());
}

void LiveSubscriber::on_custom_data(const EventSPtr &event) {
    auto context = engine_->executor_;
    for (const auto &strategy : engine_->strategies_) {
        strategy->on_custom_data(context, event->msg_type(), event->data_as_bytes(), event->data_length(),
                                 event->source());
    }
}

void LiveSubscriber::on_backtest_sync_signal(const EventSPtr &event) {
    const auto &signal = event->data<BacktestSyncSignal>();
    if (signal.flag == BacktestSyncSignal::MarketData) {
        auto id = journal::JIDUtil::build(journal::JIDUtil::TD_REQ);
        BacktestSyncSignal signal_to_send;
        signal_to_send.flag = BacktestSyncSignal::MatchOrder;
        engine_->writers_[id]->write(infra::time::now_time(), signal_to_send);
    } else if (signal.flag == BacktestSyncSignal::MatchOrder) {
        auto id = journal::JIDUtil::build(journal::JIDUtil::MD_REQ);
        BacktestSyncSignal signal_to_send;
        signal_to_send.flag = BacktestSyncSignal::MarketData;
        engine_->writers_[id]->write(infra::time::now_time(), signal_to_send);
    }
}

} // namespace btra