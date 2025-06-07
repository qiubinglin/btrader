#pragma once

#include "eventengine.h"
#include "strategy.h"

namespace btra {

/**
 * @brief Only support one account for now.
 *
 */
class CPEngine : public EventEngine {
private:
    void react() override;
    void on_setup() override;
    void on_active() override;
    std::string name() const override { return "cp"; }

    void add_strategy(strategy::StrategySPtr strat);

    void pre_start();
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
    void post_stop();
    void on_custom_data(const EventSPtr &event);

    template <typename OnMethod = void (strategy::Strategy::*)(strategy::ExecutorSPtr &)>
    void invoke(OnMethod method) {
        auto context = std::dynamic_pointer_cast<strategy::Executor>(executor_);
        for (const auto &strategy : strategies_) {
            (*strategy.*method)(context);
        }
    }

    template <typename TradingData,
              typename OnMethod = void (strategy::Strategy::*)(strategy::ExecutorSPtr &, const TradingData &)>
    void invoke(OnMethod method, const TradingData &data) {
        auto context = std::dynamic_pointer_cast<strategy::Executor>(executor_);
        for (const auto &strategy : strategies_) {
            (*strategy.*method)(context, data);
        }
    }

    template <typename TradingData1, typename TradingData2,
              typename OnMethod = void (strategy::Strategy::*)(strategy::ExecutorSPtr &, const TradingData1 &,
                                                               const TradingData2 &)>
    void invoke(OnMethod method, const TradingData1 &data1, const TradingData2 &data2) {
        auto context = std::dynamic_pointer_cast<strategy::Executor>(executor_);
        for (const auto &strategy : strategies_) {
            (*strategy.*method)(context, data1, data2);
        }
    }

    template <typename TradingData1, typename TradingData2,
              typename OnMethod = void (strategy::Strategy::*)(strategy::ExecutorSPtr &, const TradingData1 &,
                                                               const TradingData2 &, JID)>
    void invoke(OnMethod method, const TradingData1 &data1, const TradingData2 &data2, JID source) {
        auto context = std::dynamic_pointer_cast<strategy::Executor>(executor_);
        for (const auto &strategy : strategies_) {
            (*strategy.*method)(context, data1, data2, source);
        }
    }

    template <typename TradingData,
              typename OnMethod = void (strategy::Strategy::*)(strategy::ExecutorSPtr &, const TradingData &, JID)>
    void invoke(OnMethod method, const TradingData &data, JID source) {
        auto context = std::dynamic_pointer_cast<strategy::Executor>(executor_);
        for (const auto &strategy : strategies_) {
            (*strategy.*method)(context, data, source);
        }
    }

    template <typename OnMethod = void (strategy::Strategy::*)(strategy::ExecutorSPtr &, uint32_t,
                                                               const std::vector<uint8_t> &, uint32_t, JID)>
    void invoke(OnMethod method, uint32_t msg_type, const std::vector<uint8_t> &data, uint32_t length, JID source) {
        auto context = std::dynamic_pointer_cast<strategy::Executor>(executor_);
        for (const auto &strategy : strategies_) {
            (*strategy.*method)(context, msg_type, data, length, source);
        }
    }

private:
    strategy::ExecutorSPtr executor_;
    std::vector<strategy::StrategySPtr> strategies_;

    infra::DLHelper dlhelper_;
    bool pre_start_ = false;
    int64_t trading_daytime_end_ = 0;
    unsigned trading_msg_count_ = 0;
    unsigned md_account_count_ = 0;
};

} // namespace btra