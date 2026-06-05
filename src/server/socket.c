#include "server/socket.h"
#include <stdio.h>
#include <sys/types.h>  // 提供基本系统数据类型（如 pid_t, size_t）
#include <sys/socket.h> // 提供 socket(), bind(), connect(), accept() 等核心函数和结构体
#include <netinet/in.h> // 提供 IPv4/IPv6 的地址结构体（如 sockaddr_in）
#include <arpa/inet.h>  // 提供 IP 地址转换函数（如 inet_pton(), inet_ntoa()）
#include <unistd.h>     // 提供 close() 函数来关闭 socket
#include <asm-generic/socket.h>
#include <string.h>
#include <stdlib.h>

int init_socket(int *fd, char *ip, char *port)
{
    *fd = socket(AF_INET, SOCK_STREAM, 0);
    if (*fd == -1)
    {
    }
    // 端口复用
    int opt = 1;
    int ret = setsockopt(*fd, SOL_SOCKET, SO_REUSEPORT, &opt, sizeof(opt));

    struct sockaddr_in addr;
    memset(&addr, 0, sizeof(addr));

    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = inet_addr(ip);
    addr.sin_port = htons(atoi(port));

    ret = bind(*fd, (struct sockaddr *)&addr, sizeof(addr));

    ret = listen(*fd, 10);
    return 0;
}