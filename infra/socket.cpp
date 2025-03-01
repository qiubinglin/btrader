#include "socket.h"

#include <stdexcept>

#include "format.h"

namespace infra::nng {

void Socket::setsockopt(int level, int option, const void *optval, size_t optvallen) const {
  int rc = nn_setsockopt(sock_, level, option, optval, optvallen);
  if (rc != 0) {
    throw std::runtime_error("can not setsockopt");
  }
}

void Socket::getsockopt(int level, int option, void *optval, size_t *optvallen) const {
  int rc = nn_getsockopt(sock_, level, option, optval, optvallen);
  if (rc != 0) {
    throw std::runtime_error("can not getsockopt");
  }
}

int Socket::bind(const std::string &path) {
  url_ = "ipc://" + path;
  int rc = nn_bind(sock_, url_.c_str());
  if (rc < 0) {
    throw std::runtime_error(fmt::format("can not bind to {}", url_.c_str()));
  }
  return rc;
}

int Socket::connect(const std::string &path) {
  url_ = "ipc://" + path;
  int rc = nn_connect(sock_, url_.c_str());
  if (rc < 0) {
    throw std::runtime_error(fmt::format("can not connect to {}", url_.c_str()));
  }
  return rc;
}

void Socket::shutdown(int how) const {
  int rc = nn_shutdown(sock_, how);
  if (rc != 0) {
    throw std::runtime_error("can not shutdown");
  }
}

void Socket::close() const {
  int rc = nn_close(sock_);
  if (rc != 0) {
    throw std::runtime_error("can not close");
  }
}

int Socket::send(const std::string &msg, int flags) const {
  int rc = nn_send(sock_, msg.c_str(), msg.length(), flags);
  if (rc < 0) {
    if (nn_errno() != EAGAIN) {
      throw std::runtime_error(
          fmt::format("can not send to {} errno [{}] {}", url_.c_str(), nn_errno(), nn_strerror(nn_errno())));
    }
    return -1;
  }
  return rc;
}

int Socket::recv(int flags) {
  int rc = nn_recv(sock_, buf_.data(), buf_.size(), flags);
  if (rc < 0) {
    switch (nn_errno()) {
      case ETIMEDOUT:
      case EAGAIN:
        break;
      case EINTR: {
        //   SPDLOG_WARN("interrupted when receiving from [{}]", url_);
        break;
      }
      default: {
        throw std::runtime_error(
            fmt::format("can not send to {} errno [{}] {}", url_.c_str(), nn_errno(), nn_strerror(nn_errno())));
      }
    }
    message_.assign(buf_.data(), 0);
    return 0;
  } else {
    message_.assign(buf_.data(), rc);
    return rc;
  }
}

const std::string &Socket::recv_msg(int flags) {
  recv(flags);
  return message_;
}

}  // namespace infra::nng