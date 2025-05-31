#ifndef _INFRA_EPOLL_H_
#define _INFRA_EPOLL_H_

#include <errno.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/epoll.h>
#include <sys/eventfd.h>
#include <sys/wait.h>
#include <unistd.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Create epoll instance.
 *
 * @param flags 0 or EFD_CLOEXEC, EFD_CLOEXEC means to close fd automatically when process terminating.
 * @return int epoll fd, or -1 means error.
 */
int create_epoll(int flags);

/**
 * @brief Create a counter object in kernel.
 *
 * @param initval
 * @param flags EFD_NONBLOCK
 * @return int fd for the counter object. -1 when error.
 */
int create_eventfd(unsigned int initval, int flags);

/**
 * @brief Create a epoll event object
 *
 * @param event_fd eventfd
 * @param flags EPOLLIN: trigger when readable, EPOLLET: Edge-Triggered
 * @return struct epoll_event which exchange information between user space and kernel space.
 */
struct epoll_event create_epoll_event(int event_fd, int flags);

#ifdef __cplusplus
}
#endif

#endif