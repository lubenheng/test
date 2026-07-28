#include "utility.h"
#include "string.h"
#include "ctype.h"
#include <stddef.h>

/**
 * @brief  字符串转无符号长整型（支持 8/10/16 进制自动识别）
 * @param  cp:    输入字符串指针
 * @param  endp:  输出参数，返回解析结束的位置
 * @param  base:  进制（2~16），传 0 则自动判断
 * @return 转换后的无符号长整型数值
 */
static unsigned long cmd_data_strtoul(const char* cp, char** endp, unsigned int base)
{
    unsigned long result = 0;
    unsigned long value;

    // 处理以 0 开头的数字（八进制 / 十六进制前缀）
    if (*cp == '0')
    {
        cp++;

        // 0x 开头 → 强制识别为 16 进制
        if ((*cp == 'x') && isxdigit(cp[1]))
        {
            base = 16;
            cp++;
        }
        // 未指定进制 → 默认按 8 进制处理
        else if (!base)
        {
            base = 8;
        }
    }

    // 进制仍未指定 → 默认 10 进制
    if (!base)
    {
        base = 10;
    }

    // 逐字符解析：合法字符 → 转换为数值
    while (isxdigit(*cp) &&
           (value = isdigit(*cp) ? *cp - '0' :
           (islower(*cp) ? toupper(*cp) : *cp) - 'A' + 10) < base)
    {
        result = result * base + value;
        cp++;
    }

    // 如果传入 endp，记录解析结束位置
    if (endp)
    {
        *endp = (char*)cp;
    }

    return result;
}

/**
 * @brief  对外接口：字符串转无符号长整型
 * @param  buf: 输入字符串（支持 123 / 0x123）
 * @return 无符号长整型
 */
unsigned long get_arg_ulong(char* buf)
{
    unsigned long ret = 0;
    int base = 10;
    char *ptr = buf;

    // 识别 0x / 0X 前缀 → 16 进制
    if (!strncmp(buf, "0x", 2) || !strncmp(buf, "0X", 2))
    {
        base = 16;
        ptr = buf + 2;
    }

    // 调用底层转换函数
    ret = cmd_data_strtoul((const char*)ptr, 0, base);

    return ret;
}

/**
 * @brief  字符串转有符号长整型（支持正负号 + 自动进制）
 * @param  cp:    输入字符串
 * @param  endp:  解析结束位置
 * @param  base:  进制
 * @return 有符号长整型
 */
static long cmd_data_strtol(const char* cp, char** endp, unsigned int base)
{
    long result = 0;
    int sign = 1;       // 符号：1=正，-1=负
    int value;

    // 跳过开头空白符
    while (isspace((unsigned char)*cp))
    {
        cp++;
    }

    // 处理正负号
    if (*cp == '-')
    {
        sign = -1;
        cp++;
    }
    else if (*cp == '+')
    {
        cp++;
    }

    // 处理 0 开头（八进制 / 十六进制）
    if (*cp == '0')
    {
        cp++;

        // 0x / 0X → 16 进制
        if ((*cp == 'x' || *cp == 'X') && isxdigit((unsigned char)cp[1]))
        {
            base = 16;
            cp++;
        }
        // 未指定进制 → 默认 8 进制
        else if (!base)
        {
            base = 8;
        }
    }

    // 未指定进制 → 默认 10 进制
    if (!base)
    {
        base = 10;
    }

    // 逐字符转换
    while (isxdigit((unsigned char)*cp))
    {
        if (isdigit((unsigned char)*cp))
        {
            value = *cp - '0';
        }
        else if (islower((unsigned char)*cp))
        {
            value = toupper((unsigned char)*cp) - 'A' + 10;
        }
        else
        {
            value = *cp - 'A' + 10;
        }

        // 超出进制范围 → 停止解析
        if (value >= (int)base)
        {
            break;
        }

        result = result * base + value;
        cp++;
    }

    // 记录结束位置
    if (endp)
    {
        *endp = (char*)cp;
    }

    // 加上符号返回
    return sign * result;
}

/**
 * @brief  对外接口：字符串转有符号长整型
 * @param  buf: 输入字符串（支持 +123 / -456 / 0xAB）
 * @return 有符号长整型
 */
signed long get_arg_long(char* buf)
{
    long ret = 0;
    int base = 10;
    char *ptr = buf;
    int sign = 1;

    // 跳过空白
    while (isspace((unsigned char)*ptr))
    {
        ptr++;
    }

    // 识别符号
    if (*ptr == '-')
    {
        sign = -1;
        ptr++;
    }
    else if (*ptr == '+')
    {
        ptr++;
    }

    // 识别 0x 十六进制
    if (!strncasecmp(ptr, "0x", 2))
    {
        base = 16;
        ptr += 2;
    }

    // 调用解析函数
    ret = cmd_data_strtol(ptr, 0, base);

    // 应用符号
    if (sign < 0 && ret > 0)
    {
        ret = -ret;
    }

    return ret;
}

