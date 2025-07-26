#include "mentor.h"
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

static int server_sock = -1;

// 通过Unix域套接字发送文件描述符
int send_fd(int socket, int fd_to_send) {
    struct msghdr msg = {0};
    struct cmsghdr *cmsg;
    char buf[CMSG_SPACE(sizeof(int))];
    char dummy_data = 'X';
    struct iovec io = {.iov_base = &dummy_data, .iov_len = 1};

    msg.msg_iov = &io;
    msg.msg_iovlen = 1;
    msg.msg_control = buf;
    msg.msg_controllen = sizeof(buf);

    cmsg = CMSG_FIRSTHDR(&msg);
    cmsg->cmsg_level = SOL_SOCKET;
    cmsg->cmsg_type = SCM_RIGHTS;
    cmsg->cmsg_len = CMSG_LEN(sizeof(int));

    *((int *)CMSG_DATA(cmsg)) = fd_to_send;
    msg.msg_controllen = CMSG_SPACE(sizeof(int));

    return sendmsg(socket, &msg, 0);
}

namespace btra {

void Mentor::send_eventfds(const std::vector<int> &eventfd_list, const std::string &socket_path) {
#ifndef HP
    const char *SOCKET_PATH = socket_path.c_str();

    int client_sock;
    struct sockaddr_un addr;

    // 创建Unix域套接字服务器
    server_sock = socket(AF_UNIX, SOCK_STREAM, 0);
    if (server_sock == -1) {
        perror("socket");
        exit(1);
    }

    memset(&addr, 0, sizeof(addr));
    addr.sun_family = AF_UNIX;
    strncpy(addr.sun_path, SOCKET_PATH, sizeof(addr.sun_path) - 1);

    unlink(SOCKET_PATH);

    if (bind(server_sock, (struct sockaddr *)&addr, sizeof(addr)) == -1) {
        perror("bind");
        exit(1);
    }

    if (listen(server_sock, 1) == -1) {
        perror("listen");
        exit(1);
    }

    printf("[server] Waiting for client...\n");

    // Waiting for connection
    client_sock = accept(server_sock, NULL, NULL);
    if (client_sock == -1) {
        perror("accept");
        exit(1);
    }

    printf("[server] client is connected\n");

    // Send eventfd to client
    for (auto eventfd_fd : eventfd_list) {
        if (send_fd(client_sock, eventfd_fd) == -1) {
            perror("send_fd");
            exit(1);
        }
    }

    sleep(2);
    close(client_sock);
#endif
}

} // namespace btra