#include "infra/epoll_usage.h"

/**
 * @brief Create an epoll instance for monitoring file descriptors
 * 
 * This function is a thin wrapper around the Linux epoll_create1() system call.
 * It creates a new epoll instance that can monitor multiple file descriptors
 * for various I/O events efficiently.
 * 
 * @param flags Epoll creation flags (0 or EPOLL_CLOEXEC)
 * @return Epoll file descriptor on success, -1 on failure
 * 
 * @note epoll_create1() is the modern replacement for epoll_create() and
 *       provides better control over file descriptor flags.
 */
int create_epoll(int flags) {
    return epoll_create1(flags);
}

/**
 * @brief Create an event file descriptor for signaling events
 * 
 * This function is a thin wrapper around the Linux eventfd() system call.
 * It creates a kernel counter object that can be used to signal events
 * between processes or from user space to kernel space.
 * 
 * @param initval Initial value for the counter (typically 0 or 1)
 * @param flags Event file descriptor flags (EFD_NONBLOCK, EFD_CLOEXEC, etc.)
 * @return Event file descriptor on success, -1 on failure
 * 
 * @note Event file descriptors are atomic and thread-safe, making them
 *       ideal for inter-thread communication in event loops.
 */
int create_eventfd(unsigned int initval, int flags) { 
    return eventfd(initval, flags); 
}

/**
 * @brief Create and configure an epoll event structure
 * 
 * This function creates an epoll_event structure and configures it to monitor
 * a specific file descriptor for the specified events. The structure is ready
 * to be used with epoll_ctl() to add the file descriptor to an epoll instance.
 * 
 * @param event_fd The file descriptor to monitor for events
 * @param flags Event flags to monitor (EPOLLIN, EPOLLOUT, EPOLLET, etc.)
 * @return Configured epoll_event structure
 * 
 * @note The epoll_event structure contains:
 *       - ev.data.fd: The file descriptor to monitor
 *       - ev.events: The events to monitor (bitwise OR of flags)
 *       
 *       Common usage patterns:
 *       - EPOLLIN | EPOLLET: Edge-triggered read events
 *       - EPOLLIN | EPOLLOUT: Monitor both read and write readiness
 *       - EPOLLIN | EPOLLERR | EPOLLHUP: Monitor read, errors, and hang-ups
 */
struct epoll_event create_epoll_event(int event_fd, int flags) {
    struct epoll_event ev;
    
    // Set the file descriptor to monitor
    ev.data.fd = event_fd;
    
    // Set the events to monitor (bitwise OR of flags)
    ev.events = flags;
    
    return ev;
}

/**
 * @brief Create a complete epoll monitoring setup
 * 
 * This convenience function creates both an epoll instance and an event file
 * descriptor, returning them as a paired structure. This is useful for setting
 * up complete event monitoring systems where you need both the monitoring
 * capability and a way to wake up the event loop from external sources.
 * 
 * The function creates:
 * 1. An epoll instance for monitoring file descriptors
 * 2. An event file descriptor for signaling events
 * 
 * @param epfd_flags Flags for epoll creation (0 or EPOLL_CLOEXEC)
 * @param efd_initval Initial value for the event file descriptor counter
 * @param flags Flags for the event file descriptor (EFD_NONBLOCK, etc.)
 * @return epoll_pair_t structure containing both file descriptors
 * 
 * @note This function provides a complete event monitoring setup in one call.
 *       The returned file descriptors should be properly managed:
 *       - Close them when no longer needed to prevent leaks
 *       - Use the epfd with epoll_ctl() to add file descriptors to monitor
 *       - Use the efd to signal events (e.g., write a value to wake up epoll_wait)
 *       
 * @example
 *   // Create epoll pair for a worker thread pool
 *   struct epoll_pair_t pair = create_epoll_pair(0, 0, EFD_NONBLOCK);
 *   
 *   // Add the eventfd to the epoll instance for monitoring
 *   struct epoll_event ev = create_epoll_event(pair.efd, EPOLLIN);
 *   epoll_ctl(pair.epfd, EPOLL_CTL_ADD, pair.efd, &ev);
 *   
 *   // In another thread, signal an event:
 *   uint64_t value = 1;
 *   write(pair.efd, &value, sizeof(value));
 */
struct epoll_pair_t create_epoll_pair(int epfd_flags, unsigned int efd_initval, int flags) {
    struct epoll_pair_t epoll_pair;
    
    // Create the epoll instance for monitoring file descriptors
    epoll_pair.epfd = create_epoll(epfd_flags);
    
    // Create the event file descriptor for signaling events
    epoll_pair.efd = create_eventfd(efd_initval, flags);
    
    return epoll_pair;
}