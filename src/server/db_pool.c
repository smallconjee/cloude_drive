#include "server/db_pool.h"
#include "commen/logger.h"
#include "commen/config.h"
#include <stdio.h>
#include <stdlib.h>

// 初始化连接池
int db_pool_init(db_pool_t *db_pool)
{
    server_config_t conf;
    int ret = read_config("../conf/server.conf", &conf);
    if (ret == -1)
    {
        LOG_WARNING("读取配置文件失败");
        return -1;
    }
    for (int i = 0; i < MAX_DB_CONNS; i++)
    {
        db_pool->conn[i] = mysql_init(NULL);
        if (db_pool->conn[i] == NULL)
        {
            LOG_ERROR("mysql_init()失败");
            mysql_close(db_pool->conn[i]);
            return -1;
        }
        MYSQL *tmp_conn = mysql_real_connect(db_pool->conn[i], "localhost", conf.mysql_user, conf.mysql_password, "cloud_drive", 3306, NULL, 0);
        if (tmp_conn == NULL)
        {

            LOG_ERROR("mysql_connect() 失败: %s", mysql_error(db_pool->conn[i]));
            mysql_close(db_pool->conn[i]); // 释放内存
            return -1;
        }
        db_pool->conn[i] = tmp_conn;
    }

    // 初始化剩余连接数
    db_pool->free_count = MAX_DB_CONNS;

    // 初始化锁
    ret = pthread_mutex_init(&db_pool->lock, NULL);
    if (ret != 0)
    {
        LOG_ERROR("连接池初始化锁失败");
        return -1;
    }

    // 初始化条件变量
    ret = pthread_cond_init(&db_pool->cond, NULL);
    if (ret != 0)
    {
        LOG_ERROR("条件变量初始化失败");
        return -1;
    }
    return 0;
}

// 从连接池里拿出一个数据库
MYSQL *db_pool_get(db_pool_t *db_pool)
{
    if (db_pool == NULL)
    {
        LOG_WARNING("你给我传空指针啊你,你真行!大拇哥");
        return NULL;
    }
    int ret = pthread_mutex_lock(&db_pool->lock);
    if (ret != 0)
    {
        LOG_ERROR("锁坏了,赶快查查什么情况");
        return NULL;
    }

    // 如果可用数据库为0,阻塞等待
    while (db_pool->free_count == 0)
    {
        ret = pthread_cond_wait(&db_pool->cond, &db_pool->lock);
    }

    db_pool->free_count--;
    MYSQL *conn = db_pool->conn[db_pool->free_count];
    db_pool->conn[db_pool->free_count] = NULL;
    pthread_mutex_unlock(&db_pool->lock);

    // 检查数据库是否还是连接状态,如果未连接则报错
    if (mysql_ping(conn) != 0)
    {
        LOG_WARNING("数据库断开了 需要重新连接一下");
        mysql_close(conn);
        return NULL;
    }
    return conn;
}

// 用完还连接池
void db_pool_put(db_pool_t *db_pool, MYSQL *conn)
{
    // 先上锁
    int ret = pthread_mutex_lock(&db_pool->lock);
    if (ret != 0)
    {
        LOG_ERROR("锁不上了,快去检查一下");
        return;
    }

    // 如果连接池是满的说明不用进行还的操作
    if (db_pool->free_count == MAX_DB_CONNS)
    {
        LOG_WARNING("连接池是满的你还他干嘛?");
        mysql_close(conn);
        pthread_mutex_unlock(&db_pool->lock);
        return;
    }

    // 把还的数据库放到空闲数组上
    db_pool->conn[db_pool->free_count] = conn;
    db_pool->free_count++;
    pthread_mutex_unlock(&db_pool->lock);
    pthread_cond_signal(&db_pool->conn);
}

// 收回连接池
void db_pool_destroy(db_pool_t *db_pool)
{
    if (db_pool == NULL)
    {
        LOG_WARNING("你敢给我传空的啊?哎吆喂,干嘛呢你!");
        return;
    }
    // 先上锁
    int ret = pthread_mutex_lock(&db_pool->lock);
    if (ret != 0)
    {
        LOG_ERROR("小兄弟,快去查查锁咋回事,上不上了");
        return;
    }
    for (int i = 0; i < MAX_DB_CONNS; i++)
    {
        if (db_pool->conn[i] != NULL)
        {
            mysql_close(db_pool->conn[i]);
            db_pool->conn[i] = NULL;
        }
    }
    pthread_mutex_unlock(&db_pool->lock);

    // 销毁锁
    pthread_mutex_destroy(&db_pool->lock);
    pthread_cond_destroy(&db_pool->cond);
}
