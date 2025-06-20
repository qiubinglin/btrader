#include "strategy/executor.h"

namespace btra::strategy {

Executor::Executor(EventEngine *engine) { set_engine(engine); }

void Executor::set_engine(EventEngine *engine) { engine_ = engine; }

const Book &Executor::book() const { return book_; }

Book &Executor::book() { return book_; }

int64_t Executor::now_event_time() const { return engine_->now_event_time(); }

} // namespace btra::strategy