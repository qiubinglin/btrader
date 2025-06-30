#pragma once

#include "core/eventengine.h"
#include "cp/backtest_dump.h"
#include "strategy/strategy.h"

namespace btra {

class LiveSubscriber;
class BacktestSubscriber;

/**
 * @brief Only support one account for now.
 *
 */
class CPEngine : public EventEngine {
private:
    ~CPEngine();
    void react() override;
    void on_setup() override;
    void on_active() override;
    std::string name() const override { return "cp"; }

    void add_strategy(strategy::StrategySPtr strat);

    void pre_start();
    void post_stop();

    bool is_backtest() const { return main_cfg_.get_backtest_data_type() != enums::BacktestDataType::None; }

private:
    strategy::ExecutorSPtr executor_;
    std::vector<strategy::StrategySPtr> strategies_;

    infra::DLHelper dlhelper_;
    bool pre_start_ = false;
    int64_t trading_daytime_end_ = 0;
    unsigned trading_msg_count_ = 0;
    unsigned md_account_count_ = 0;

    LiveSubscriber *live_subscriber_{nullptr};
    BacktestSubscriber *backtest_subscriber_{nullptr}; /* Not use now */

    BacktestDump backtest_dump_;

    friend class LiveSubscriber;
    friend class BacktestSubscriber;
    friend class Invoker;
};

} // namespace btra