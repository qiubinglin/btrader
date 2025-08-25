#include "socket.h"

#include <stdexcept>

#include "format.h"

namespace infra::nng {

/**
 * @brief Set socket options using the NNG API
 * 
 * This method wraps nn_setsockopt() and provides exception-based error handling.
 * Common options include buffer sizes, timeouts, and protocol-specific settings.
 * 
 * @param level Protocol level (e.g., NN_SOL_SOCKET for socket-level options)
 * @param option Specific option to set
 * @param optval Pointer to the option value
 * @param optvallen Size of the option value in bytes
 * @throws std::runtime_error if nn_setsockopt() fails
 */
void Socket::setsockopt(int level, int option, const void *optval, size_t optvallen) const {
    int rc = nn_setsockopt(sock_, level, option, optval, optvallen);
    if (rc != 0) {
        throw std::runtime_error("can not setsockopt");
    }
}

/**
 * @brief Get socket options using the NNG API
 * 
 * This method wraps nn_getsockopt() and provides exception-based error handling.
 * Used to retrieve current socket configuration values.
 * 
 * @param level Protocol level (e.g., NN_SOL_SOCKET for socket-level options)
 * @param option Specific option to retrieve
 * @param optval Pointer to store the retrieved option value
 * @param optvallen Pointer to the size of the option value
 * @throws std::runtime_error if nn_getsockopt() fails
 */
void Socket::getsockopt(int level, int option, void *optval, size_t *optvallen) const {
    int rc = nn_getsockopt(sock_, level, option, optval, optvallen);
    if (rc != 0) {
        throw std::runtime_error("can not getsockopt");
    }
}

/**
 * @brief Bind the socket to a local IPC endpoint
 * 
 * Creates a local IPC endpoint and binds the socket to it. The path is
 * automatically prefixed with "ipc://" to form a valid NNG URL.
 * 
 * @param path Local filesystem path for the IPC endpoint
 * @return Endpoint ID on success (positive integer)
 * @throws std::runtime_error if nn_bind() fails
 * 
 * @note The endpoint will be created if it doesn't exist
 */
int Socket::bind(const std::string &path) {
    // Construct the full IPC URL by prefixing the path
    url_ = "ipc://" + path;
    
    // Bind the socket to the local endpoint
    int rc = nn_bind(sock_, url_.c_str());
    if (rc < 0) {
        throw std::runtime_error(fmt::format("can not bind to {}", url_.c_str()));
    }
    return rc;
}

/**
 * @brief Connect the socket to a remote IPC endpoint
 * 
 * Connects to an existing IPC endpoint. The path is automatically
 * prefixed with "ipc://" to form a valid NNG URL.
 * 
 * @param path Remote filesystem path for the IPC endpoint
 * @return Endpoint ID on success (positive integer)
 * @throws std::runtime_error if nn_connect() fails
 * 
 * @note The remote endpoint must already exist (be bound by another process)
 */
int Socket::connect(const std::string &path) {
    // Construct the full IPC URL by prefixing the path
    url_ = "ipc://" + path;
    
    // Connect to the remote endpoint
    int rc = nn_connect(sock_, url_.c_str());
    if (rc < 0) {
        throw std::runtime_error(fmt::format("can not connect to {}", url_.c_str()));
    }
    return rc;
}

/**
 * @brief Shutdown the socket
 * 
 * Shuts down the socket in the specified direction. This is useful for
 * graceful termination of communication.
 * 
 * @param how Shutdown mode:
 *            - 0: Shutdown receive operations
 *            - 1: Shutdown send operations  
 *            - 2: Shutdown both directions
 * @throws std::runtime_error if nn_shutdown() fails
 */
void Socket::shutdown(int how) const {
    int rc = nn_shutdown(sock_, how);
    if (rc != 0) {
        throw std::runtime_error("can not shutdown");
    }
}

/**
 * @brief Close the socket
 * 
 * Closes the underlying NNG socket and frees all associated resources.
 * This should be called when the socket is no longer needed.
 * 
 * @throws std::runtime_error if nn_close() fails
 */
void Socket::close() const {
    int rc = nn_close(sock_);
    if (rc != 0) {
        throw std::runtime_error("can not close");
    }
}

/**
 * @brief Send a message through the socket
 * 
 * Sends a message to the connected endpoint(s). By default, uses non-blocking
 * mode (NN_DONTWAIT) to avoid blocking the calling thread.
 * 
 * @param msg The message string to send
 * @param flags Send flags (default: NN_DONTWAIT for non-blocking)
 * @return Number of bytes sent on success, -1 if operation would block
 * @throws std::runtime_error if send fails (except for EAGAIN which indicates blocking)
 * 
 * @note EAGAIN errors (operation would block) are not treated as exceptions
 *       and return -1 instead, allowing the caller to handle blocking gracefully
 */
int Socket::send(const std::string &msg, int flags) const {
    int rc = nn_send(sock_, msg.c_str(), msg.length(), flags);
    if (rc < 0) {
        // EAGAIN indicates the operation would block (non-blocking mode)
        // This is not an error condition, just that no data could be sent immediately
        if (nn_errno() != EAGAIN) {
            throw std::runtime_error(
                fmt::format("can not send to {} errno [{}] {}", url_.c_str(), nn_errno(), nn_strerror(nn_errno())));
        }
        return -1;
    }
    return rc;
}

/**
 * @brief Receive a message from the socket
 * 
 * Receives a message from the socket and stores it in the internal buffer.
 * By default, uses non-blocking mode (NN_DONTWAIT) to avoid blocking.
 * 
 * @param flags Receive flags (default: NN_DONTWAIT for non-blocking)
 * @return Number of bytes received on success, 0 if operation would block
 * @throws std::runtime_error if receive fails (except for timeout/interrupt conditions)
 * 
 * @note The received message is stored in the internal message_ member
 *       and can be accessed via last_message()
 */
int Socket::recv(int flags) {
    // Receive data into the internal buffer
    int rc = nn_recv(sock_, buf_.data(), buf_.size(), flags);
    if (rc < 0) {
        // Handle different error conditions
        switch (nn_errno()) {
        case ETIMEDOUT:
            // Timeout occurred (normal in non-blocking mode)
            break;
        case EAGAIN:
            // Operation would block (normal in non-blocking mode)
            break;
        case EINTR: {
            // Operation was interrupted by a signal
            // This is typically not an error condition
            //   SPDLOG_WARN("interrupted when receiving from [{}]", url_);
            break;
        }
        default: {
            // Unexpected error - throw exception
            throw std::runtime_error(
                fmt::format("can not send to {} errno [{}] {}", url_.c_str(), nn_errno(), nn_strerror(nn_errno())));
        }
        }
        
        // Clear the message buffer on error/timeout
        message_.assign(buf_.data(), 0);
        return 0;
    } else {
        // Successfully received data - store it in the message buffer
        message_.assign(buf_.data(), rc);
        return rc;
    }
}

/**
 * @brief Receive a message and return it as a string
 * 
 * Convenience method that calls recv() and returns the received message.
 * This eliminates the need to call recv() and then last_message() separately.
 * 
 * @param flags Receive flags (default: NN_DONTWAIT for non-blocking)
 * @return Reference to the received message string
 * 
 * @note If recv() fails or times out, an empty string may be returned
 */
const std::string &Socket::recv_msg(int flags) {
    recv(flags);
    return message_;
}

} // namespace infra::nng