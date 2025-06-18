#include "eventengine.h"

#include <limits>

namespace btra {

EventEngine::EventEngine() {
    begin_time_ = infra::time::now_time();
    end_time_ = std::numeric_limits<int64_t>::max();
}

void EventEngine::setcfg(const Json::json &cfg) { cfg_ = cfg; }

void EventEngine::run() {
    setup();
    continual_ = true;
    events_.connect(cs_); /* Start event loop. */
}

void EventEngine::step() {
    continual_ = false;
    events_.connect(cs_); /* Start event loop. */
}

void EventEngine::stop() { live_ = false; }

void EventEngine::setup() {
    on_setup();
    ob_helper_.add_customer(reader_); // Add reader_ to observe helper for not HP mode.
    events_ = rx::observable<>::create<EventSPtr>([this](auto &s) { this->produce(s); }).publish();
    react();
    live_ = true;
}

void EventEngine::produce(const rx::subscriber<EventSPtr> &sb) {
    try {
        do {
            on_active();
            live_ = drain(sb) && live_;
        } while (continual_ and live_);
    } catch (...) {
        live_ = false;
        sb.on_error(std::current_exception());
    }
    if (not live_) {
        sb.on_completed();
    }
}

bool EventEngine::drain(const rx::subscriber<EventSPtr> &sb) {
#ifndef HP
    if (live_ and ob_helper_.data_available()) {
#endif
        while (live_ and reader_->data_available()) {
            if (reader_->current_frame()->gen_time() <= end_time_) {
                int64_t frame_time = reader_->current_frame()->gen_time();
                if (frame_time > now_event_time_) {
                    now_event_time_ = frame_time;
                }
                sb.on_next(reader_->current_frame());
                reader_->next();
            } else {
                INFRA_LOG_INFO("reached defined end time {}",
                               infra::time::strftime(reader_->current_frame()->gen_time()));
                return false;
            }
        }
#ifndef HP
    }
#endif
    return true;
}

} // namespace btra