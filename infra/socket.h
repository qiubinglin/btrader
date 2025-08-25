#pragma once

/**
 * @file socket.h
 * @brief NNG (nanomsg) socket wrapper for inter-process communication
 * 
 * This header provides a C++ wrapper around the NNG socket API, which supports
 * various messaging patterns like pipeline, pub/sub, and request/reply.
 * The wrapper handles common socket operations with proper error handling.
 */

#include <nng/compat/nanomsg/nn.h>
#include <nng/compat/nanomsg/pipeline.h>
#include <nng/compat/nanomsg/pubsub.h>
#include <nng/compat/nanomsg/reqrep.h>

#include <string>
#include <vector>

/** Maximum message length for socket communication (16KB) */
#define MAX_MSG_LENGTH (16 * 1024)

namespace infra::nng {

/**
 * @brief NNG socket wrapper class for simplified socket operations
 * 
 * This class provides a high-level interface to NNG sockets, handling
 * common operations like binding, connecting, sending, and receiving messages.
 * It automatically manages the underlying socket descriptor and provides
 * error handling with exceptions.
 * 
 * Supported socket types (via NNG):
 * - Pipeline (push/pull)
 * - Publish/Subscribe
 * - Request/Reply
 */
class Socket {
public:
    /**
     * @brief Set socket options
     * 
     * @param level The protocol level (e.g., NN_SOL_SOCKET)
     * @param option The option to set
     * @param optval Pointer to the option value
     * @param optvallen Size of the option value
     * @throws std::runtime_error if the operation fails
     */
    void setsockopt(int level, int option, const void *optval, size_t optvallen) const;

    /**
     * @brief Get socket options
     * 
     * @param level The protocol level (e.g., NN_SOL_SOCKET)
     * @param option The option to retrieve
     * @param optval Pointer to store the option value
     * @param optvallen Pointer to the size of the option value
     * @throws std::runtime_error if the operation fails
     */
    void getsockopt(int level, int option, void *optval, size_t *optvallen) const;

    /**
     * @brief Bind the socket to a local endpoint
     * 
     * Creates an IPC endpoint and binds the socket to it.
     * The path will be prefixed with "ipc://" automatically.
     * 
     * @param path Local path for the IPC endpoint
     * @return The endpoint ID on success
     * @throws std::runtime_error if binding fails
     */
    int bind(const std::string &path);

    /**
     * @brief Connect the socket to a remote endpoint
     * 
     * Connects to an existing IPC endpoint.
     * The path will be prefixed with "ipc://" automatically.
     * 
     * @param path Remote path for the IPC endpoint
     * @return The endpoint ID on success
     * @throws std::runtime_error if connection fails
     */
    int connect(const std::string &path);

    /**
     * @brief Shutdown the socket
     * 
     * @param how Shutdown mode (0 = read, 1 = write, 2 = both)
     * @throws std::runtime_error if shutdown fails
     */
    [[maybe_unused]] void shutdown(int how = 0) const;

    /**
     * @brief Close the socket
     * 
     * Closes the underlying NNG socket and frees associated resources.
     * @throws std::runtime_error if closing fails
     */
    void close() const;

    /**
     * @brief Send a message through the socket
     * 
     * @param msg The message to send
     * @param flags Send flags (default: NN_DONTWAIT for non-blocking)
     * @return Number of bytes sent, or -1 if operation would block
     * @throws std::runtime_error if send fails (except for EAGAIN)
     */
    int send(const std::string &msg, int flags = NN_DONTWAIT) const;

    /**
     * @brief Receive a message from the socket
     * 
     * @param flags Receive flags (default: NN_DONTWAIT for non-blocking)
     * @return Number of bytes received, or 0 if operation would block
     * @throws std::runtime_error if receive fails (except for timeout/interrupt)
     */
    int recv(int flags = NN_DONTWAIT);

    /**
     * @brief Receive a message and return it as a string
     * 
     * Convenience method that calls recv() and returns the received message.
     * 
     * @param flags Receive flags (default: NN_DONTWAIT for non-blocking)
     * @return Reference to the received message string
     */
    const std::string &recv_msg(int flags = NN_DONTWAIT);

    /**
     * @brief Get the current socket URL
     * 
     * @return The socket URL (with "ipc://" prefix)
     */
    [[nodiscard]] const std::string &get_url() const { return url_; };

    /**
     * @brief Get the last received message
     * 
     * @return Reference to the last received message
     */
    [[nodiscard]] const std::string &last_message() const { return message_; };

private:
    int sock_;                    ///< NNG socket descriptor
    std::string url_;             ///< Current socket URL (ipc://path)
    std::vector<char> buf_;       ///< Receive buffer for messages
    std::string message_;         ///< Last received message
};

} // namespace infra::nng