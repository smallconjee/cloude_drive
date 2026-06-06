#pragma once

#include <mysql/mysql.h>
#include <pthread.h>

#define MAX_DB_CONNS 10 // 连接池最大数量
typedef struct db_pool
{
    MYSQL *conn[MAX_DB_CONNS];
    int free_count;
    pthread_mutex_t lock;
    pthread_cond_t cond;
} db_pool_t;

// 初始化连接池
int db_pool_init(db_pool_t *db_pool);

// 从连接池里拿出一个数据库
MYSQL *db_pool_get(db_pool_t *db_pool);

// 用完还连接池
void db_pool_put(db_pool_t *db_pool, MYSQL *conn);

// 收回连接池
void db_pool_destroy(db_pool_t *db_pool);
