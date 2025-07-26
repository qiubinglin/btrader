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

// 全局变量用于清理
static int server_sock = -1;
static int eventfd_fd = -1;

// 信号处理函数
void cleanup_handler(int sig) {
    printf("\n[服务器] 收到信号，开始清理资源...\n");
    if (server_sock != -1) close(server_sock);
    if (eventfd_fd != -1) close(eventfd_fd);
    unlink(SOCKET_PATH);
    exit(0);
}

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

    *((int *) CMSG_DATA(cmsg)) = fd_to_send;
    msg.msg_controllen = CMSG_SPACE(sizeof(int));

    return sendmsg(socket, &msg, 0);
}

int main() {
    int client_sock;
    struct sockaddr_un addr;
    uint64_t counter = 1;
    
    // 设置信号处理
    signal(SIGINT, cleanup_handler);
    signal(SIGTERM, cleanup_handler);
    
    printf("=== EventFD服务器进程 ===\n");
    
    // 创建eventfd
    eventfd_fd = eventfd(0, EFD_CLOEXEC);
    if (eventfd_fd == -1) {
        perror("eventfd");
        exit(1);
    }
    printf("[服务器] 创建eventfd成功，fd = %d\n", eventfd_fd);
    
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
    
    if (bind(server_sock, (struct sockaddr*)&addr, sizeof(addr)) == -1) {
        perror("bind");
        exit(1);
    }
    
    if (listen(server_sock, 1) == -1) {
        perror("listen");
        exit(1);
    }
    
    printf("[服务器] 等待客户端连接... (请运行 ./client)\n");
    
    // 等待客户端连接
    client_sock = accept(server_sock, NULL, NULL);
    if (client_sock == -1) {
        perror("accept");
        exit(1);
    }
    
    printf("[服务器] 客户端已连接\n");
    
    // 发送eventfd到客户端
    if (send_fd(client_sock, eventfd_fd) == -1) {
        perror("send_fd");
        exit(1);
    }
    
    printf("[服务器] eventfd已发送给客户端\n");
    printf("[服务器] 开始发送数据...\n");
    
    // 定期向eventfd写入数据
    for (int i = 1; i <= 10; i++) {
        sleep(2);
        counter = i;
        if (write(eventfd_fd, &counter, sizeof(counter)) == -1) {
            perror("write to eventfd");
            break;
        }
        printf("[服务器] 向eventfd写入数据: %lu\n", counter);
    }
    
    // 发送结束信号
    counter = 999; // 特殊值表示结束
    write(eventfd_fd, &counter, sizeof(counter));
    printf("[服务器] 发送结束信号\n");
    
    sleep(2);
    close(client_sock);
    cleanup_handler(0);
    
    return 0;
}