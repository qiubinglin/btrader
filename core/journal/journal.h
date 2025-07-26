#pragma once

#include "infra/epoll_usage.h"
#include "jid.h"
#include "jlocation.h"
#include "page.h"

namespace btra::journal {

/**
 * @brief Journal class, the abstraction of continuous memory access
 */
class Journal {
public:
    Journal(JLocationSPtr location, uint32_t dest_id, bool is_writing, bool lazy)
        : location_(std::move(location)),
          dest_id_(dest_id),
          is_writing_(is_writing),
          lazy_(lazy),
          frame_(std::shared_ptr<FrameUnit>(new FrameUnit())),
          page_frame_nb_(0u) {}

    ~Journal();

    /**
     * @brief Current frame is the one that can be read or written.
     *
     * @return FrameUnitSPtr&
     */
    [[nodiscard]] FrameUnitSPtr &current_frame() { return frame_; }

    /**
     * @brief Current page is the one that contains current frame.
     *
     * @return PageUnitSPtr&
     */
    [[nodiscard]] PageUnitSPtr &current_page() { return page_; }

    [[nodiscard]] const JLocationSPtr &get_location() const { return location_; }

    /**
     * @brief Get location unique id.
     * 
     * @return uint32_t 
     */
    [[maybe_unused]] [[nodiscard]] uint32_t get_source() const { return location_->location_uid; }

    [[maybe_unused]] [[nodiscard]] uint32_t get_dest() const { return dest_id_; }

    /**
     * @brief Move current frame to the next available one
     */
    void next();

    /**
     * @brief Makes sure after this call, next time user calls current_frame() gets the right available frame
     * (gen_time() > nanotime or writable)
     * @param nanotime
     */
    void seek_to_time(int64_t nanotime);

private:
    /* journal unique id: [location_, dest_id_]. */
    const JLocationSPtr location_; /* Indicate a journal directory, location_ is part of journal unique id. */
    const uint32_t dest_id_;       /* Indicate a journal file, dest_id_ is part of journal unique id. Make it as unique? */

    const bool is_writing_; /* Read-only or write-only */
    const bool lazy_;

    PageUnitSPtr page_;      /* Current page. */
    FrameUnitSPtr frame_;    /* Current frame. */
    uint64_t page_frame_nb_; /* Current frame number in page. */

    /**
     * @brief Load page of page_id
     * 
     * @param page_id 
     */
    void load_page(int page_id);

    /**
     * @brief load next page, current page will be released if not empty
     *
     */
    void load_next_page();

    friend class Reader;

    friend class Writer;
};

class JourIndicator {
public:
    JourIndicator();
    JourIndicator(int efd) : efd_(efd) {}
    ~JourIndicator();
    int init();
    int post();
    void set_fd(int efd) { efd_ = efd; }
    int get_fd() const { return efd_; }

private:
    int efd_{-1};

    friend class JourObserver;
};

#define MAX_EVENTS 10

class JourObserver {
public:
    JourObserver();
    ~JourObserver();
    int init();
    int add_target(int efd);
    int wait();
    void handle();

private:
    int epfd_{-1};
    int coming_event_num_{0};
    size_t events_received_{0};
    struct epoll_event events_[MAX_EVENTS];
};

} // namespace btra::journal