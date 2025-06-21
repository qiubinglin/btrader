#pragma once

#include <rxcpp/rx.hpp>
#include <unordered_map>

#include "core/observe_helper.h"
#include "enums.h"
#include "event.h"
#include "ext_scheduler.h"
#include "main_cfg.h"
#include "reader.h"
#include "uid_util.h"
#include "writer.h"

namespace rx = rxcpp;

namespace btra {

template <MsgTag::Tag tag>
inline bool is(const EventSPtr &event) {
    return event->msg_type() == tag;
}

inline bool over_max_tag(const EventSPtr &event) { return event->msg_type() >= MsgTag::TAG_MAX_SIZE; }

#define ON_MEM_FUNC(func) [this](const EventSPtr &event) { this->func(event); }
#define ON_MEM_OBJ(obj, func) [this](const EventSPtr &event) { this->obj->func(event); }

using WriterMap = std::unordered_map<uint32_t, journal::WriterUPtr>;
class EventEngine {
public:
    EventEngine();
    virtual ~EventEngine() = default;

    /**
     * @brief Set the configuration for the event engine before setup().
     *
     * @param cfg The configuration in JSON format.
     */
    void setcfg(const Json::json &cfg);

    /**
     * @brief Run the event loop.
     *
     */
    void run();

    /**
     * @brief Step the event loop once. This method is for testing or debugging purposes.
     *
     */
    void step();

    /**
     * @brief Stop the event loop.
     *
     */
    void stop();

    /**
     * @brief Setup the event engine. This method is called before the event loop starts.
     *
     */
    void setup();

    int64_t now_event_time() const { return now_event_time_; }

    WriterMap &writers() { return writers_; }
    journal::Writer *get_writer(uint32_t id) { return writers_.at(id).get(); }

    const MainCfg &get_main_cfg() const { return main_cfg_; }
    virtual std::string name() const { return "EventEngine"; }

protected:
    /**
     * @brief The main event loop to produce events to the subscriber.
     *
     * @param sb The subscriber to which events are produced.
     */
    void produce(const rx::subscriber<EventSPtr> &sb);

    /**
     * @brief Drain the current event stream, reading events from the journal and pushing them to the subscriber.
     *
     * @param sb The subscriber to which events are pushed.
     * @return false if reached defined end time, true otherwise.
     */
    virtual bool drain(const rx::subscriber<EventSPtr> &sb);

    /**
     * @brief React to the events. This method should be implemented by derived classes to define the event handling
     * logic.
     *
     */
    virtual void react() = 0;

    /**
     * @brief Callback on setup(), derived classes can define their setup logic.
     *
     */
    virtual void on_setup() {}

    /**
     * @brief Callback on produce() event loop.
     *
     * This method can be overridden by derived classes to define custom behavior when the event engine is active.
     */
    virtual void on_active() {}

protected:
    rx::connectable_observable<EventSPtr> events_;
    rx::composite_subscription cs_;
    volatile bool continual_ = true;
    volatile bool live_ = false;

    int64_t begin_time_ = 0; /* Begin time of the event engine. Need synchronize? */
    int64_t end_time_ = 0;   /* End time of the event engine */
    int64_t now_event_time_ = 0;
    journal::ReaderUPtr reader_;
    WriterMap writers_;

    MainCfg main_cfg_;
    Json::json cfg_;

    ObserveHelper ob_helper_; /* For not hp mode. */

    friend class ExtScheduler;
};

} // namespace btra