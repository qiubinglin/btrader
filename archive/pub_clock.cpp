#include "pub_clock.h"

#include "infra/infra.h"

namespace btra {

int64_t PubClock::now() const {
  switch (mode_) {
    case Mode::REAL:
      return infra::time::now_time();
      break;
    case Mode::RECORD:
    default:
      return record_now_;
      break;
  }
}

void PubClock::set_record_now(int64_t val) { record_now_ = val; }

}  // namespace btra