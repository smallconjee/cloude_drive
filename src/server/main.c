#include "server/epoll.h"
#include "server/queue.h"
#include "server/socket.h"
#include "server/worker.h"
#include "server/thread_pool.h"

#include <stdio.h>      // printf
#include <stdlib.h>     // exit
#include <string.h>     // sizeof(buf) 清零等（若用到 memset）
#include <unistd.h>     // pipe, fork, write, read, close, setpgid
#include <signal.h>     // signal, SIGINT
#include <pthread.h>    // pthread_mutex_lock, pthread_cond_broadcast, pthread_join, pthread_exit
#include <sys/types.h>  // wait, pid_t
#include <sys/wait.h>   // wait
#include <sys/epoll.h>  // epoll_create, epoll_wait, struct epoll_event
#include <sys/socket.h> // accept

int pfd[2];

void func(int num)
{
    printf("num:%d\n", num);
    write(pfd[1], "1", 1);
}

int main(int argc, char *argv[])
{
    // 创建匿名管道
    pipe(pfd);
    // 父进程
    if (fork() != 0)
    {
        signal(SIGINT, func);
        wait(NULL);
        exit(0);
    }

    // 子进程
    // 让子进程脱离父进程,可以让子进程单独成为一个进程组
    setpgid(0, 0);

    // 初始化线程池
    thread_pool_t pool;
    init_thread_pool(&pool, 10);

    // 初始化socket的流程代码
    int sockfd = 0;
    init_socket(&sockfd, "0.0.0.0", "12345");

    int epfd = epoll_create(1);
    add_epoll_fd(epfd, sockfd);

    add_epoll_fd(epfd, pfd[0]);

    while (1)
    {
        struct epoll_event ep_arr[10];
        int nready = epoll_wait(epfd, ep_arr, 10, -1);
        for (int i = 0; i < nready; i++)
        {
            int fd = ep_arr[i].data.fd;
            if (fd == pfd[0])
            {
                char buf[10] = {0};
                read(pfd[0], buf, sizeof(buf));

                int ret = pthread_mutex_lock(&pool.lock);
                pool.exitFlag = 1;

                ret = pthread_cond_broadcast(&pool.cond);

                for (size_t idx = 0; idx < pool.thread_num; ++idx)
                {
                    pthread_join(pool.thread_id[idx], NULL);
                }
            }
        }
    }
}