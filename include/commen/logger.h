#pragma once

#include <stdio.h>
#include <stdarg.h>
#include <time.h>

typedef enum
{
    LOG_LEVEL_DEBUG = 0,
    LOG_LEVEL_INFO,
    LOG_LEVEL_WARNING,
    LOG_LEVEL_ERROR
} log_level;

// 全局日志级别
extern log_level g_log_level;

// 初始化日志系统
void init_log(const char *log_file_path);

// 关闭日志系统（释放资源）
void close_log(void);

// 清除系统可能存在的同名宏，防止冲突
#undef LOG_DEBUG
#undef LOG_INFO
#undef LOG_WARNING
#undef LOG_ERROR

// 内部核心写入函数
void write_log(log_level level, const char *file, int line, const char *func, const char *format, ...);

// 日志调用宏：使用 __FILE__ 和 __LINE__ 自动追踪代码位置
#define LOG_DEBUG(format, ...) write_log(LOG_LEVEL_DEBUG, __FILE__, __LINE__, __FUNCTION__, format, ##__VA_ARGS__)
#define LOG_INFO(format, ...) write_log(LOG_LEVEL_INFO, __FILE__, __LINE__, __FUNCTION__, format, ##__VA_ARGS__)
#define LOG_WARNING(format, ...) write_log(LOG_LEVEL_WARNING, __FILE__, __LINE__, __FUNCTION__, format, ##__VA_ARGS__)
#define LOG_ERROR(format, ...) write_log(LOG_LEVEL_ERROR, __FILE__, __LINE__, __FUNCTION__, format, ##__VA_ARGS__)