#include "fds_map.h"
/* main header above */

#include <errno.h>
#include <signal.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/eventfd.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>
#include <stdexcept>

// 通过Unix域套接字接收文件描述符
int recv_fd(int socket) {
    struct msghdr msg;
    struct cmsghdr *cmsg;
    char buf[CMSG_SPACE(sizeof(int))];
    char dummy_data;
    struct iovec io = {.iov_base = &dummy_data, .iov_len = 1};

    msg.msg_name = 0;
    msg.msg_iov = &io;
    msg.msg_iovlen = 1;
    msg.msg_control = buf;
    msg.msg_controllen = sizeof(buf);

    if (recvmsg(socket, &msg, 0) < 0) {
        return -1;
    }

    cmsg = CMSG_FIRSTHDR(&msg);
    if (cmsg && cmsg->cmsg_level == SOL_SOCKET && cmsg->cmsg_type == SCM_RIGHTS) {
        return *((int *)CMSG_DATA(cmsg));
    }

    return -1;
}

namespace btra {

void FdsMap::fix_fds(std::vector<std::string> &fds_vec, const std::string &socket_path) {
    const char *SOCKET_PATH = socket_path.c_str();

    int client_sock;
    struct sockaddr_un addr;

    // 连接到服务器
    client_sock = socket(AF_UNIX, SOCK_STREAM, 0);
    if (client_sock == -1) {
        throw std::runtime_error("Create socket failed");
    }

    memset(&addr, 0, sizeof(addr));
    addr.sun_family = AF_UNIX;
    strncpy(addr.sun_path, SOCKET_PATH, sizeof(addr.sun_path) - 1);

    if (connect(client_sock, (struct sockaddr *)&addr, sizeof(addr)) == -1) {
        throw std::runtime_error("connect - please launch server first!!!");
    }

    printf("Connect to server\n");

    for (size_t i = 0; i < fds_vec.size() / 2; ++i) {
        int received_eventfd = recv_fd(client_sock);
        if (received_eventfd == -1) {
            throw std::runtime_error("[client] - receive eventfd failed");
        }
        printf("recv %lu eventfd: %d\n", i, received_eventfd);
        fds_vec[i * 2 + 1] = std::to_string(received_eventfd);
    }

    close(client_sock);
}

} // namespace btra