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