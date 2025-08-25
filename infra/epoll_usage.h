#ifndef _INFRA_EPOLL_H_
#define _INFRA_EPOLL_H_

/**
 * @file epoll_usage.h
 * @brief Linux epoll event handling utilities for efficient I/O multiplexing
 * 
 * This header provides a simplified interface to Linux epoll functionality,
 * which is a scalable I/O event notification mechanism. It includes utilities
 * for creating epoll instances, event file descriptors, and managing epoll events.
 * 
 * Epoll is particularly efficient for handling many file descriptors compared
 * to select() or poll(), making it ideal for high-performance servers and
 * event-driven applications.
 */

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
 * @brief Structure containing an epoll instance and an event file descriptor
 * 
 * This structure pairs an epoll file descriptor with an event file descriptor,
 * providing a convenient way to manage both components together. The epoll
 * instance monitors file descriptors for events, while the eventfd provides
 * a way to signal events from user space to the epoll loop.
 * 
 * @var epfd The epoll file descriptor for monitoring events
 * @var efd The event file descriptor for signaling events
 */
typedef struct epoll_pair_t {
    int epfd;    ///< Epoll file descriptor for event monitoring
    int efd;     ///< Event file descriptor for signaling events
} epoll_pair_t;

/**
 * @brief Create an epoll instance for monitoring file descriptors
 *
 * Creates a new epoll instance that can monitor multiple file descriptors
 * for various events (readable, writable, errors, etc.). Epoll is more
 * efficient than select() or poll() for large numbers of file descriptors.
 *
 * @param flags Epoll creation flags:
 *              - 0: No special flags
 *              - EPOLL_CLOEXEC: Close the file descriptor automatically
 *                when the process terminates (useful for security)
 * 
 * @return On success: epoll file descriptor (positive integer)
 *         On failure: -1 (check errno for specific error)
 * 
 * @note The returned file descriptor should be closed with close() when
 *       no longer needed to prevent file descriptor leaks.
 */
int create_epoll(int flags);

/**
 * @brief Create an event file descriptor for signaling events
 *
 * Creates a kernel counter object that can be used to signal events
 * between processes or from user space to kernel space. When written to,
 * it increments the counter; when read from, it returns the current
 * counter value and resets it to zero.
 *
 * @param initval Initial value for the counter (typically 0 or 1)
 * @param flags Event file descriptor flags:
 *              - 0: No special flags
 *              - EFD_NONBLOCK: Make read/write operations non-blocking
 *              - EFD_CLOEXEC: Close automatically on exec (process replacement)
 *              - EFD_SEMAPHORE: Semaphore-like behavior (read returns 1, not counter value)
 * 
 * @return On success: event file descriptor (positive integer)
 *         On failure: -1 (check errno for specific error)
 * 
 * @note Event file descriptors are commonly used with epoll to wake up
 *       event loops from other threads or processes.
 */
int create_eventfd(unsigned int initval, int flags);

/**
 * @brief Create an epoll event structure for monitoring file descriptors
 *
 * Creates and configures an epoll_event structure that defines what events
 * to monitor on a specific file descriptor. This structure is used when
 * adding file descriptors to an epoll instance with epoll_ctl().
 *
 * @param event_fd The file descriptor to monitor for events
 * @param flags Event flags to monitor:
 *              - EPOLLIN: Monitor for read events (data available to read)
 *              - EPOLLOUT: Monitor for write events (space available to write)
 *              - EPOLLERR: Monitor for error events
 *              - EPOLLHUP: Monitor for hang-up events (connection closed)
 *              - EPOLLET: Edge-triggered mode (only notify on state changes)
 *              - EPOLLONESHOT: One-shot mode (remove after first event)
 * 
 * @return Configured epoll_event structure ready for use with epoll_ctl()
 * 
 * @note The returned structure contains the event_fd in ev.data.fd and
 *       the specified flags in ev.events.
 */
struct epoll_event create_epoll_event(int event_fd, int flags);

/**
 * @brief Create a complete epoll monitoring setup
 *
 * Convenience function that creates both an epoll instance and an event
 * file descriptor, returning them as a paired structure. This is useful
 * for setting up event loops where you need both monitoring capabilities
 * and a way to wake up the loop from external sources.
 *
 * @param epfd_flags Flags for epoll creation (see create_epoll())
 * @param efd_initval Initial value for the event file descriptor counter
 * @param flags Flags for the event file descriptor (see create_eventfd())
 * 
 * @return epoll_pair_t structure containing both file descriptors
 * 
 * @note This function creates a complete event monitoring setup in one call.
 *       The returned file descriptors should be closed when no longer needed.
 * 
 * @example
 *   // Create epoll pair with non-blocking eventfd
 *   struct epoll_pair_t pair = create_epoll_pair(0, 0, EFD_NONBLOCK);
 *   
 *   // Use pair.epfd for monitoring other file descriptors
 *   // Use pair.efd for signaling events (e.g., from another thread)
 */
struct epoll_pair_t create_epoll_pair(int epfd_flags, unsigned int efd_initval, int flags);

#ifdef __cplusplus
}
#endif

#endif