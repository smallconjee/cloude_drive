#pragma once
/**
 * @brief 
 * 将指定的文件描述符(fd)添加到 epoll 监听红黑树中
 * @param epfd epoll 实例的文件描述符
 * @param fd 需要被监听的套接字
 * @return int 成功返回 0；失败返回 -1
 */
int add_epoll_fd(int epfd,int fd);

/**
 * @brief 
 * 从 epoll 监听红黑树中安全删除指定的文件描述符
 * @param epfd epoll 实例的文件描述符
 * @param fd 需要取消监听的套接字/文件描述符
 * @return int 成功返回 0；失败返回 -1
 */
int del_epoll_fd(int epfd,int fd);