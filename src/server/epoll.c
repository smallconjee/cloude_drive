#include "server/epoll.h"
#include <sys/epoll.h>
#include <stdio.h>
#include <unistd.h>

int add_epoll_fd(int epfd, int fd)
{
    struct epoll_event evt;
    evt.data.fd = fd;
    // 基础阶段用 EPOLLIN；后续高性能网盘架构推荐升级为 EPOLLIN | EPOLLET (边沿触发)
    evt.events = EPOLLIN;
    int ret = epoll_ctl(epfd, EPOLL_CTL_ADD, fd, &evt);
    if (ret == -1)
    {
        perror("epoll_ctl ADD failed");
        return -1;
    }
    return 0;
}

int del_epoll_fd(int epfd, int fd)
{
    struct epoll_event dummy;
    int ret = epoll_ctl(epfd, EPOLL_CTL_DEL, fd, &dummy);
    if (ret == -1)
    {
        perror("epoll_ctl DEL failed");
        return -1;
    }
    return 0;
}