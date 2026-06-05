#pragma once

#include "server/queue.h"
#include <pthread.h>
typedef struct thread_pool{
    int thread_num;//工作线程的数目
    pthread_t *thread_id;//存放每个线程id的字符指针
    queue_t que;//队列
    pthread_mutex_t lock;
    pthread_cond_t cond;//条件变量
    int exitFlag;
}thread_pool_t;

void init_thread_pool(thread_pool_t *pool,int num);