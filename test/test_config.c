#include "commen/config.h"
#include "commen/logger.h"

#include <stdio.h>

int main(void)
{
    server_config_t test;
    read_config("../conf/server.conf", &test);
    printf("ip:%s\nport:%s\nmysql_user:%s\nmysql_password:%s\n", test.ip, test.port, test.mysql_user, test.mysql_password);
    return 0;
}