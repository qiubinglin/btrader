#include "infra/epoll_usage.h"

int create_epoll(int flags) {
    return epoll_create1(flags);
}

int create_eventfd(unsigned int initval, int flags) { return eventfd(initval, flags); }

struct epoll_event create_epoll_event(int event_fd, int flags) {
    struct epoll_event ev;
    ev.data.fd = event_fd;
    ev.events = flags;
    return ev;
}

struct epoll_pair_t create_epoll_pair(int epfd_flags, unsigned int efd_initval, int flags) {
    struct epoll_pair_t epoll_pair;
    epoll_pair.epfd = create_epoll(epfd_flags);
    epoll_pair.efd = create_eventfd(efd_initval, flags);
    return epoll_pair;
}