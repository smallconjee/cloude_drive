#include "commen/logger.h"
#include <string.h>
#include <pthread.h>
#include <unistd.h>
#include <sys/syscall.h> // 用于获取真实 TID

// 默认日志级别
log_level g_log_level = LOG_LEVEL_INFO;

static const char *level_strings[] = {
    "DEBUG",
    "INFO",
    "WARNING",
    "ERROR"};

// 全局变量
static FILE *log_fd = NULL;
static pthread_mutex_t log_mutex = PTHREAD_MUTEX_INITIALIZER; // 静态初始化锁

// 初始化日志系统
void init_log(const char *log_file_path)
{
    // 如果之前已经静态初始化了锁，这里只需打开文件
    log_fd = fopen(log_file_path, "a");
    if (log_fd == NULL)
    {
        printf("[WARNING] 无法打开日志文件 %s, 日志将直接输出到屏幕\n", log_file_path);
    }
}

// 关闭日志系统
void close_log(void)
{
    pthread_mutex_lock(&log_mutex);
    if (log_fd != NULL)
    {
        fclose(log_fd);
        log_fd = NULL;
    }
    pthread_mutex_unlock(&log_mutex);
    pthread_mutex_destroy(&log_mutex);
}

// 真正写入日志系统
void write_log(log_level level, const char *file, int line, const char *func, const char *format, ...)
{
    // 1. 过滤掉低于全局级别的日志
    if (level < g_log_level)
        return;

    // 2. 获取当前系统时间
    time_t t = time(NULL);
    struct tm *tm_info = localtime(&t);
    char time_buf[64] = {0};
    strftime(time_buf, sizeof(time_buf), "%Y-%m-%d %H:%M:%S", tm_info);

    // 3. 提取短文件名（去掉冗长的绝对路径）
    const char *short_file = strrchr(file, '/');
    short_file = short_file ? short_file + 1 : file;

    // 4. 获取 Linux 独一无二的内核真实线程 ID (TID)
    // pthread_self() 返回的是线程序号(地址)，sys_gettid 返回的才是系统级唯一的PID/TID
    pid_t tid = (pid_t)syscall(SYS_gettid);

    // 5. 确定输出方向
    FILE *target_fd = (log_fd != NULL) ? log_fd : stdout;

    // 6. 颜色控制（仅当输出到控制台 stdout 时生效）
    const char *color_start = "";
    const char *color_end = "";

    if (target_fd == stdout)
    {
        switch (level)
        {
        case LOG_LEVEL_DEBUG:
            color_start = "\033[34m";
            break; // 蓝色
        case LOG_LEVEL_INFO:
            color_start = "\033[32m";
            break; // 绿色
        case LOG_LEVEL_WARNING:
            color_start = "\033[33m";
            break; // 黄色
        case LOG_LEVEL_ERROR:
            color_start = "\033[31m";
            break; // 红色
        }
        color_end = "\033[0m"; // 重置颜色
    }

    // ================= 核心加锁临界区 =================
    pthread_mutex_lock(&log_mutex);

    // 打印日志头部：新增了 [TID:%d]
    fprintf(target_fd, "%s[%s] [TID:%d] [%s] [%s:%d %s] ",
            color_start, time_buf, tid, level_strings[level], short_file, line, func);

    // 打印用户自定义的格式化内容 (...)
    va_list args;
    va_start(args, format);
    vfprintf(target_fd, format, args);
    va_end(args);

    // 换行并重置颜色
    fprintf(target_fd, "%s\n", color_end);

    pthread_mutex_unlock(&log_mutex);
    // ================= 临界区结束 =====================

    // 7. 磁盘刷新（安全优化）：
    // 将非常耗时的 fflush 移出锁保护区。这样即使磁盘写满了或者很卡，
    // 也不会霸占锁导致快池/慢池其他线程跟着一起卡死。
    if (target_fd != stdout)
    {
        fflush(target_fd);
    }
}