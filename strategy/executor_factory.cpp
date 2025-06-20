#include "strategy/executor.h"
/* Above is main header. */

#include "strategy/backtest_executor.h"
#include "strategy/live_executor.h"

namespace btra::strategy {

ExecutorSPtr Executor::create(enums::RunMode mode, EventEngine *engine) {
    switch (mode) {
        case enums::RunMode::BACKTEST:
            return std::make_shared<BacktestExecutor>(engine);
        case enums::RunMode::LIVE:
            return std::make_shared<LiveExecutor>(engine);
        default:
            throw std::runtime_error("Unsupported run mode for strategy executor.");
    }
}

} // namespace btra::strategy