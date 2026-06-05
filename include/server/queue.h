#pragma once

// 队列节点
typedef struct node_s
{
    int fd;
    struct node_s *pNext;
} node_t;

// 定义队列

typedef struct queue_s
{
    node_t *head;
    node_t *end;
    int size;
} queue_t;

/**
 * @brief
 * 入队列
 * @param pQueue 队列
 * @param fd 活跃的fd
 * @return int 成功0 失败-1
 */
int enqueue(queue_t *pQueue, int fd);

/**
 * @brief
 * 出队列
 * @param pQueue 队列
 * @return int 成功0 失败-1
 */
int deQueue(queue_t *pQueue);
