#pragma once
#include <stddef.h>
// 这两个函数不需要知道 JWT 的具体实现细节，外部只需要调用这两个函数即可完成 JWT 的生成和验证。

// 生成 JWT token,需要提前分配好 out_token 的内存空间，并指定 max_len 的大小,以确保生成的 token 能够被正确存储。
int generate_token(const char *username, char *out_token, size_t max_len);

// 验证 JWT token，并提取用户名到 out_username 中
int verify_token(const char *token, char *out_username, size_t max_len);
