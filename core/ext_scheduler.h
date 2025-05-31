#pragma once

#include <functional>

#include "event.h"

namespace btra {

using CBFunc = std::function<void(const EventSPtr &)>;

class EventEngine;
class ExtScheduler {
public:
    ExtScheduler(EventEngine &engine);

    /**
     * @brief Add one shot timer callback.
     * @param time when to call in seconds or nano seconds
     * @param callback callback function
     */
    void add_timer(int64_t time, const CBFunc &callback);

    /**
     * @brief Add periodically callback.
     * @param duration duration in seconds or nano seconds
     * @param callback callback function
     */
    void add_time_interval(int64_t duration, const CBFunc &callback);

private:
    EventEngine &engine_;
};

}; // namespace btra