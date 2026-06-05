#include "server/thread_pool.h"
#include "server/worker.h"
#include "server/queue.h"
#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>
#include <string.h>

void init_thread_pool(thread_pool_t *pool, int num)
{
    pool->exitFlag = 0;
    pool->thread_num = num;
    pthread_mutex_init(&pool->lock, NULL);
    pthread_cond_init(&pool->cond, NULL);
    memset(&pool->que, 0, sizeof(queue_t));
    pool->thread_id = (pthread_t *)malloc(sizeof(pthread_t) * num);
    for (int i = 0; i < num; i++)
    {
        int ret = pthread_create(&pool->thread_id[i], NULL, thread_func, (void *)pool);
        if (ret == -1)
        {
            perror("pthread_create fail\n");
        }
    }
}