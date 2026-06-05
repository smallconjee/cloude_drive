CC := gcc
CFLAGS := -Wall -g -I./include
LDFLAGS := -lpthread -lmysqlclient -lcrypt
# 源文件和目标文件
SERVER_SRCS := $(wildcard src/server/*.c) $(wildcard src/commen/*.c)
CLIENT_SRCS := $(wildcard src/client/*.c) $(wildcard src/commen/*.c)

SERVER_OBJS := $(SERVER_SRCS:.c=.o)
CLIENT_OBJS := $(CLIENT_SRCS:.c=.o)

# 默认目标
.PHONY: all clean rebuild
all: bin/server bin/client

# 编译服务端
bin/server: $(SERVER_OBJS)
	@mkdir -p bin
	$(CC) $^ -o $@ $(LDFLAGS)

# 编译客户端
bin/client: $(CLIENT_OBJS)
	@mkdir -p bin
	$(CC) $^ -o $@ $(LDFLAGS)

# 编译规则（所有的 .o 依赖同名的 .c）
%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

# 清理编译产物
clean:
	$(RM) $(SERVER_OBJS) $(CLIENT_OBJS) bin/server bin/client

# 重新编译
rebuild: clean all
