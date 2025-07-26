#include <sys/eventfd.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <stdint.h>
#include <signal.h>

#define SOCKET_PATH "/tmp/eventfd_socket"

static int received_eventfd = -1;

void cleanup_handler(int sig) {
    printf("\n[客户端] 收到信号，开始清理资源...\n");
    if (received_eventfd != -1) close(received_eventfd);
    exit(0);
}

// 通过Unix域套接字接收文件描述符
int recv_fd(int socket) {
    struct msghdr msg = {0};
    struct cmsghdr *cmsg;
    char buf[CMSG_SPACE(sizeof(int))];
    char dummy_data;
    struct iovec io = {.iov_base = &dummy_data, .iov_len = 1};

    msg.msg_iov = &io;
    msg.msg_iovlen = 1;
    msg.msg_control = buf;
    msg.msg_controllen = sizeof(buf);

    if (recvmsg(socket, &msg, 0) < 0) {
        return -1;
    }

    cmsg = CMSG_FIRSTHDR(&msg);
    if (cmsg && cmsg->cmsg_level == SOL_SOCKET && cmsg->cmsg_type == SCM_RIGHTS) {
        return *((int *) CMSG_DATA(cmsg));
    }

    return -1;
}

int main() {
    int client_sock;
    struct sockaddr_un addr;
    uint64_t value;
    
    signal(SIGINT, cleanup_handler);
    signal(SIGTERM, cleanup_handler);
    
    printf("=== EventFD客户端进程 ===\n");
    
    // 连接到服务器
    client_sock = socket(AF_UNIX, SOCK_STREAM, 0);
    if (client_sock == -1) {
        perror("socket");
        exit(1);
    }
    
    memset(&addr, 0, sizeof(addr));
    addr.sun_family = AF_UNIX;
    strncpy(addr.sun_path, SOCKET_PATH, sizeof(addr.sun_path) - 1);
    
    if (connect(client_sock, (struct sockaddr*)&addr, sizeof(addr)) == -1) {
        perror("connect - 请先启动服务器进程");
        exit(1);
    }
    
    printf("[客户端] 已连接到服务器\n");
    
    // 接收eventfd
    received_eventfd = recv_fd(client_sock);
    if (received_eventfd == -1) {
        printf("[客户端] 接收eventfd失败\n");
        exit(1);
    }
    
    printf("[客户端] 成功接收eventfd，fd = %d\n", received_eventfd);
    printf("[客户端] 开始监听eventfd事件...\n");
    
    // 持续监听eventfd事件
    while (1) {
        ssize_t bytes_read = read(received_eventfd, &value, sizeof(value));
        if (bytes_read == -1) {
            if (errno == EAGAIN || errno == EWOULDBLOCK) {
                continue;
            }
            perror("read from eventfd");
            break;
        } else if (bytes_read == 0) {
            printf("[客户端] eventfd已关闭\n");
            break;
        } else {
            printf("[客户端] 从eventfd读取到数据: %lu\n", value);
            
            if (value == 999) {
                printf("[客户端] 收到结束信号，退出\n");
                break;
            }
        }
    }
    
    close(received_eventfd);
    close(client_sock);
    
    printf("[客户端] 执行完毕\n");
    return 0;
}