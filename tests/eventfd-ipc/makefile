# Makefile for eventfd IPC example

CC = gcc
CFLAGS = -Wall -Wextra -std=c99 -D_GNU_SOURCE
TARGETS = server client eventfd_ipc

.PHONY: all clean run_demo help

all: $(TARGETS)

# 独立的服务器进程
server: server.c
	$(CC) $(CFLAGS) -o server server.c

# 独立的客户端进程  
client: client.c
	$(CC) $(CFLAGS) -o client client.c

# 单个程序包含父子进程的版本
eventfd_ipc: eventfd_ipc.c
	$(CC) $(CFLAGS) -o eventfd_ipc eventfd_ipc.c

# 运行演示 - 父子进程版本
run_demo: eventfd_ipc
	@echo "=== 运行父子进程版本演示 ==="
	./eventfd_ipc

# 清理编译结果
clean:
	rm -f $(TARGETS)
	rm -f /tmp/eventfd_socket

help:
	@echo "可用的编译目标:"
	@echo "  all        - 编译所有程序"
	@echo "  server     - 编译服务器进程"
	@echo "  client     - 编译客户端进程"
	@echo "  eventfd_ipc - 编译父子进程版本"
	@echo "  run_demo   - 运行父子进程演示"
	@echo "  clean      - 清理编译结果"
	@echo ""
	@echo "使用方法:"
	@echo "1. 父子进程版本:"
	@echo "   make eventfd_ipc && ./eventfd_ipc"
	@echo ""
	@echo "2. 独立进程版本:"
	@echo "   make server client"
	@echo "   # 在终端1中运行:"
	@echo "   ./server"
	@echo "   # 在终端2中运行:"
	@echo "   ./client"