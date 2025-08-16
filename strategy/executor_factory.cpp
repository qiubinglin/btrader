#include "strategy/executor.h"
/* Above is main header. */

#include "strategy/live_executor.h"

namespace btra::strategy {

ExecutorSPtr Executor::create(enums::RunMode mode, EventEngine *engine) {
    return std::make_shared<LiveExecutor>(engine);
}

} // namespace btra::strategy