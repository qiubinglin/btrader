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

using WriterMap = std::unordered_map<uint32_t, journal::WriterUPtr>;
class EventEngine {
public:
    EventEngine();
    virtual ~EventEngine() = default;
    void setcfg(const Json::json &cfg);
    void run();
    void step();
    void stop();

    void setup();

    int64_t now_event_time() const { return now_event_time_; }

    WriterMap &writers() { return writers_; }
    journal::Writer *get_writer(uint32_t id) { return writers_.at(id).get(); }
    ExtScheduler ext_scheduler();
    const MainCfg &get_main_cfg() const { return main_cfg_; }
    virtual std::string name() const { return "EventEngine"; }

protected:
    void produce(const rx::subscriber<EventSPtr> &sb);
    virtual bool drain(const rx::subscriber<EventSPtr> &sb);

    virtual void react() = 0;
    virtual void on_setup() {}
    virtual void on_active() {}

protected:
    rx::connectable_observable<EventSPtr> events_;
    rx::composite_subscription cs_;
    volatile bool continual_ = true;
    volatile bool live_ = false;

    int64_t begin_time_ = 0;
    int64_t end_time_ = 0;
    int64_t now_event_time_ = 0;
    journal::ReaderUPtr reader_;
    WriterMap writers_;
    MainCfg main_cfg_;

    Json::json cfg_;

    ObserveHelper ob_helper_;

    friend class ExtScheduler;
};

} // namespace btra