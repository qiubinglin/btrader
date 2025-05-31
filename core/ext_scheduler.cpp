#include "ext_scheduler.h"

#include "eventengine.h"

namespace btra {

ExtScheduler::ExtScheduler(EventEngine &engine) : engine_(engine) {}

void ExtScheduler::add_timer(int64_t time, const CBFunc &callback) {}

void ExtScheduler::add_time_interval(int64_t duration, const CBFunc &callback) {}

} // namespace btra