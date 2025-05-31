#include "journal.h"

#include "enums.h"

namespace btra::journal {

Journal::~Journal() {
    if (page_.get() != nullptr) {
        page_.reset();
    }
}

void Journal::next() {
    assert(page_.get() != nullptr);
    if (frame_->msg_type() == MsgTag::PageEnd) {
        load_next_page();
    } else {
        frame_->move_to_next();
        page_frame_nb_++;
    }
}

void Journal::seek_to_time(int64_t time) {
    int page_id = PageUnit::find_page_id(location_, dest_id_, time);
    load_page(page_id);
    while (page_->is_full() && page_->end_time() <= time) {
        load_next_page();
    }
    while (frame_->has_data() && frame_->gen_time() <= time) {
        next();
    }
}

void Journal::load_page(int page_id) {
    if (page_.get() == nullptr or page_->get_page_id() != page_id) {
        page_ = PageUnit::load(location_, dest_id_, page_id, is_writing_, lazy_);
    }
    frame_->set_address(page_->first_frame_address());
    page_frame_nb_ = 0u;
}

void Journal::load_next_page() { load_page(page_->get_page_id() + 1); }

JourIndicator::JourIndicator() {}

JourIndicator::~JourIndicator() {
    if (efd_ > 0)
        close(efd_);
}

int JourIndicator::init() {
    efd_ = create_eventfd(0, EFD_NONBLOCK);
    return efd_;
}

int JourIndicator::post() {
#ifndef HP
    if (efd_ == -1) {
        return -1;
    }
    uint64_t val = 1;
    write(efd_, &val, sizeof(val));
#endif
    return 0;
}

JourObserver::JourObserver() {}

JourObserver::~JourObserver() {
    if (epfd_ > 0)
        close(epfd_);
}

int JourObserver::init() {
    epfd_ = create_epoll(EFD_CLOEXEC);
    if (epfd_ == -1) {
        perror("create_epoll");
    }
    return epfd_;
}

int JourObserver::add_target(int efd) {
    struct epoll_event ev;
    ev.events = EPOLLIN | EPOLLET;
    ev.data.fd = efd;
    if (epoll_ctl(epfd_, EPOLL_CTL_ADD, efd, &ev) == -1) {
        perror("epoll_ctl");
        return -1;
    }
    return 0;
}

int JourObserver::wait() {
    coming_event_num_ = epoll_wait(epfd_, events_, MAX_EVENTS, -1);
    if (coming_event_num_ == -1) {
        perror("epoll_wait");
        return -1;
    }
    return coming_event_num_;
}

int JourObserver::handle() {
    for (int i = 0; i < coming_event_num_; i++) {
        int fd = events_[i].data.fd;

        // 注意：边缘触发下必须循环读光
        while (1) {
            uint64_t val;
            ssize_t s = read(fd, &val, sizeof(val));
            if (s == -1) {
                if (errno == EAGAIN || errno == EWOULDBLOCK)
                    break; // 没有更多数据，退出循环
                else {
                    perror("read");
                    exit(EXIT_FAILURE);
                }
            } else if (s == 0) {
                break; // EOF
            } else {
                // printf("Parent received event from fd %d, val=%lu\n", fd, val);
                events_received_++;
            }
        }
    }
}

} // namespace btra::journal