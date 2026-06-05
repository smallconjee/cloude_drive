#include "server/worker.h"
#include "server/queue.h"
#include "server/thread_pool.h"
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>

void *thread_func(void *arg)
{
    thread_pool_t *pool = (thread_pool_t *)arg;

    while (1)
    {
        int fd = 0;
        int ret = pthread_mutex_lock(&pool->lock);

        while (0 == pool->que.size && pool->exitFlag == 0)
        {
            ret = pthread_cond_wait(&pool->cond, &pool->lock);
        }
        if (1 == pool->exitFlag)
        {
            ret = pthread_mutex_unlock(&pool->lock);
            pthread_exit((void *)NULL);
        }

        fd = pool->que.head->fd;
        deQueue(&pool->que);
        ret = pthread_mutex_unlock(&pool->lock);

        // 接下来就可以正式开始工作了
    }
}