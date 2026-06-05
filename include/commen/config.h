#pragma once

// 定义配置结构体
typedef struct
{
    char ip[32];
    char port[8];
    char mysql_user[32];
    char mysql_password[32];
} server_config_t;

extern server_config_t g_config;

int read_config(const char *config_file_path, server_config_t *config);