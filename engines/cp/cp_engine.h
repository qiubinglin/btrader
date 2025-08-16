#pragma once

#include <memory>
#include "core/eventengine.h"
#include "engines/cp/statistics_dump.h"
#include "strategy/strategy.h"

#include "extension/depthcallboard.h"

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

private:
    strategy::ExecutorSPtr executor_;
    std::vector<strategy::StrategySPtr> strategies_;

    infra::DLHelper dlhelper_;
    bool pre_start_ = false;
    int64_t trading_daytime_end_ = 0;
    unsigned trading_msg_count_ = 0;
    unsigned md_account_count_ = 0;

    LiveSubscriber *live_subscriber_{nullptr};

    StatisticsDump statistics_dump_;
    std::unique_ptr<extension::DepthCallBoard> simulation_depth_callboard_;

    friend class LiveSubscriber;
    friend class Invoker;
};

} // namespace btra