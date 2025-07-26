#include "ext_scheduler.h"

#include "eventengine.h"

namespace btra {

ExtScheduler::ExtScheduler(EventEngine &engine) : engine_(engine) {}

void ExtScheduler::add_timer(int64_t, const CBFunc &) {}

void ExtScheduler::add_time_interval(int64_t, const CBFunc &) {}

} // namespace btra