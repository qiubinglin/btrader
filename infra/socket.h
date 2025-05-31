#pragma once

#include <nng/compat/nanomsg/nn.h>
#include <nng/compat/nanomsg/pipeline.h>
#include <nng/compat/nanomsg/pubsub.h>
#include <nng/compat/nanomsg/reqrep.h>

#include <string>
#include <vector>

#define MAX_MSG_LENGTH (16 * 1024)

namespace infra::nng {

class Socket {
public:
    void setsockopt(int level, int option, const void *optval, size_t optvallen) const;
    void getsockopt(int level, int option, void *optval, size_t *optvallen) const;

    int bind(const std::string &path);

    int connect(const std::string &path);

    [[maybe_unused]] void shutdown(int how = 0) const;

    void close() const;

    int send(const std::string &msg, int flags = NN_DONTWAIT) const;

    int recv(int flags = NN_DONTWAIT);

    const std::string &recv_msg(int flags = NN_DONTWAIT);

    [[nodiscard]] const std::string &get_url() const { return url_; };

    [[nodiscard]] const std::string &last_message() const { return message_; };

private:
    int sock_;
    std::string url_;
    std::vector<char> buf_;
    std::string message_;
};

} // namespace infra::nng