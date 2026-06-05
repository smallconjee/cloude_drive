#include "commen/config.h"
#include "commen/logger.h" // 引入你刚改好的日志系统
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>

server_config_t g_config;

// 辅助函数:去除字符串前后空格、制表符、换行符

static void trim_string(char *str)
{
    if (str == NULL)
        return;
    // 1. 去除尾部空格和换行符
    int len = strlen(str);
    while (len > 0 && (isspace((unsigned char)str[len - 1]) || str[len - 1] == '\r' || str[len - 1] == '\n'))
    {
        str[len - 1] = '\0';
        len--;
    }

    // 2.去除头部空格
    int start = 0;
    while (str[start] && isspace((unsigned char)str[start]))
    {
        start++;
    }
    if (start > 0)
    {
        memmove(str, str + start, strlen(str + start) + 1);
    }
}

int read_config(const char *config_file_path, server_config_t *config)
{
    FILE *fp = fopen(config_file_path, "r");
    if (fp == NULL)
    {
        LOG_ERROR("无法打开配置文件: %s", config_file_path);
        return -1;
    }

    char line[256];
    int line_num = 0;

    while (fgets(line, sizeof(line), fp))
    {
        line_num++;

        // 去除当前行的前后空白字符
        trim_string(line);

        // 跳过空行和注释行
        if (strlen(line) == 0 || line[0] == '#')
        {
            continue;
        }

        // 寻找等号
        char *pos = strchr(line, '=');
        if (pos == NULL)
        {
            LOG_WARNING("配置文件第 %d 行格式错误(缺失'='):%s", line_num, line);
            continue;
        }

        // 以等号为界,切断字符串
        *pos = '\0';
        char *key = line;
        char *val = pos + 1;

        // 再次去除key和val的空格
        trim_string(key);
        trim_string(val);

        // 匹配并赋值
        if (strcmp(key, "ip") == 0)
        {
            strncpy(config->ip, val, sizeof(config->ip) - 1);
        }
        else if (strcmp(key, "port") == 0)
        {
            strncpy(config->port, val, sizeof(config->ip) - 1);
        }
        else if (strcmp(key, "mysql_user") == 0)
        {
            strncpy(config->mysql_user, val, sizeof(config->mysql_user) - 1);
        }
        else if (strcmp(key, "mysql_password") == 0)
        {
            strncpy(config->mysql_password, val, sizeof(config->mysql_password) - 1);
        }
        else
        {
            LOG_WARNING("未知的配置项 [%s] 于第 %d 行", key, line_num);
        }
    }
    fclose(fp);
    LOG_INFO("配置文件 %s 加载成功。", config_file_path);
    return 0;
}