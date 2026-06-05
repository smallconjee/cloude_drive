#include "commen/jwt.h"
#include "commen/logger.h"
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <l8w8jwt/encode.h>
#include <l8w8jwt/decode.h>
int generate_token(const char *username, char *out_token, size_t max_len)
{
    // 用于存储生成的JWT字符串
    char *jwt = NULL;
    size_t jwt_length = 0;

    // 初始化编码参数结构体
    struct l8w8jwt_encoding_params params;
    l8w8jwt_encoding_params_init(&params);

    // 设置JWT使用的算法,这里用的HS512
    params.alg = L8W8JWT_ALG_HS512;

    // 设置JWT的主要载荷部分
    params.sub = "cloud_drive"; // 主题
    params.sub_length = strlen(params.sub);

    params.iss = "smallconjee"; // 签发者
    params.iss_length = strlen(params.iss);

    params.aud = "cloud_drive_client"; // 接收方
    params.aud_length = strlen(params.aud);

    // token过期/有效时间
    params.iat = time(NULL);
    params.exp = time(NULL) + 7200; // 两小时后token过期

    // 设置加密密钥
    params.secret_key = (unsigned char *)"snow string token key";
    params.secret_key_length = strlen((char *)params.secret_key);

    // 输出变量
    params.out = &jwt;
    params.out_length = &jwt_length;

    // 加密
    int ret = l8w8jwt_encode(&params);

    if (ret != L8W8JWT_SUCCESS)
    {
        if (jwt)
        {
            l8w8jwt_free(jwt);
        }
        LOG_WARNING("%s用户token加密失败", username);
        return -1;
    }

    if (jwt_length < max_len)
    {
        strcpy(out_token, jwt);
        l8w8jwt_free(jwt);
        LOG_INFO("%s用户token加密成功", username);
        return 0;
    }

    if (jwt)
    {
        l8w8jwt_free(jwt);
    }
    LOG_WARNING("%s用户token加密失败", username);
    return -1;
}

/**
 * @brief 验证 JWT Token 并提取用户名
 * @param token 待验证的 JWT 字符串
 * @param out_username 用于存储解析出的用户名的缓冲区
 * @param max_len 目标缓冲区 out_username 的最大长度
 * @return int 成功返回 0，失败返回 -1
 */
#include <stdio.h>
#include <string.h>
#include <l8w8jwt/decode.h>

/**
 * @brief 验证 JWT Token 并提取用户名
 * @param token 待验证的 JWT 字符串
 * @param out_username 用于存储解析出的用户名的缓冲区
 * @param max_len 目标缓冲区 out_username 的最大长度
 * @return int 成功返回 0，失败返回 -1
 */
int verify_token(const char *token, char *out_username, size_t max_len)
{
    // 1. 入参合法性检查
    if (token == NULL || out_username == NULL || max_len == 0)
    {
        LOG_ERROR("Token验证失败: 输入参数无效（指针为空或缓冲区长度为0）");
        return -1;
    }

    LOG_INFO("开始验证 Token...");

    // 2. 初始化解密与验证所需的参数结构体
    struct l8w8jwt_decoding_params params;
    l8w8jwt_decoding_params_init(&params);

    // 配置算法与待解析的 JWT 原始字符串
    params.alg = L8W8JWT_ALG_HS512;
    params.jwt = (char *)token;
    params.jwt_length = strlen(token);

    // 配置服务器签名密钥（必须与加密时保持绝对一致）
    char *key = "snow string token key";
    params.verification_key = (unsigned char *)key;
    params.verification_key_length = strlen(key);

    // 3. 定义变量用以接收解析出的 Claims（载荷键值对）和验证状态
    struct l8w8jwt_claim *claims = NULL;
    size_t claim_count = 0;
    enum l8w8jwt_validation_result validation_result;

    // 4. 调用库函数执行解密与校验（包含签名验证、过期时间验证等）
    int decode_result = l8w8jwt_decode(&params, &validation_result, &claims, &claim_count);

    // 5. 校验结果判断
    if (decode_result == L8W8JWT_SUCCESS && validation_result == L8W8JWT_VALID)
    {
        LOG_INFO("Token 解密及签名验证通过。有效载荷数量: %zu", claim_count);

        // 6. 遍历 Claims 数组，寻找注册键 "sub" (通常用来存放用户名或用户 ID)
        for (size_t i = 0; i < claim_count; i++)
        {
            if (strcmp(claims[i].key, "sub") == 0)
            {
                // 7. 安全检查：防止用户名过长导致目标缓冲区溢出
                if (strlen(claims[i].value) < max_len)
                {
                    strcpy(out_username, claims[i].value);
                    LOG_INFO("Token 验证成功。当前登录用户: %s", out_username);

                    // 成功提取数据，释放 claims 动态内存并安全退出
                    if (claims)
                    {
                        l8w8jwt_free_claims(claims, claim_count);
                    }
                    return 0;
                }
                else
                {
                    LOG_WARN("接收用户名的目标缓冲区太小 (当前用户名长度: %zu, 缓冲区最大限制: %zu)",
                             strlen(claims[i].value), max_len);
                }
            }
        }
        LOG_WARN("Token 有效，但未在 Payload 中找到 'sub' 字段");
    }
    else
    {
        // 校验失败，输出具体的解密状态码与验证错误码（例如：1代表Token过期，2代表签名不匹配）
        LOG_ERROR("Token 验证失败! 解密返回值: %d, 验证错误码: %d",
                  decode_result, validation_result);
    }

    // 8. 兜底清理：在失败或异常退出路径下，统一释放 claims 内存，防止内存泄漏
    if (claims)
    {
        l8w8jwt_free_claims(claims, claim_count);
        LOG_INFO("异常退出路径：成功释放 claims 内存");
    }

    return -1;
}