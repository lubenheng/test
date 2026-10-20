#include "utility.h"
#include "string.h"
#include "ctype.h"
#include <stddef.h>

/**
 * @brief  �ַ���ת�޷��ų����ͣ�֧�� 8/10/16 �����Զ�ʶ��
 * @param  cp:    �����ַ���ָ��
 * @param  endp:  ������������ؽ���������λ��
 * @param  base:  ���ƣ�2~16������ 0 ���Զ��ж�
 * @return ת������޷��ų�������ֵ
 */
static unsigned long cmd_data_strtoul(const char* cp, char** endp, unsigned int base)
{
    unsigned long result = 0;
    unsigned long value;

    // ������ 0 ��ͷ�����֣��˽��� / ʮ������ǰ׺��
    if (*cp == '0')
    {
        cp++;

        // 0x ��ͷ �� ǿ��ʶ��Ϊ 16 ����
        if ((*cp == 'x') && isxdigit(cp[1]))
        {
            base = 16;
            cp++;
        }
        // δָ������ �� Ĭ�ϰ� 8 ���ƴ���
        else if (!base)
        {
            base = 8;
        }
    }

    // ������δָ�� �� Ĭ�� 10 ����
    if (!base)
    {
        base = 10;
    }

    // ���ַ��������Ϸ��ַ� �� ת��Ϊ��ֵ
    while (isxdigit(*cp) &&
           (value = isdigit(*cp) ? *cp - '0' :
           (islower(*cp) ? toupper(*cp) : *cp) - 'A' + 10) < base)
    {
        result = result * base + value;
        cp++;
    }

    // ������� endp����¼��������λ��
    if (endp)
    {
        *endp = (char*)cp;
    }

    return result;
}

/**
 * @brief  ����ӿڣ��ַ���ת�޷��ų�����
 * @param  buf: �����ַ�����֧�� 123 / 0x123��
 * @return �޷��ų�����
 */
unsigned long get_arg_ulong(char* buf)
{
    unsigned long ret = 0;
    int base = 10;
    char *ptr = buf;

    // ʶ�� 0x / 0X ǰ׺ �� 16 ����
    if (!strncmp(buf, "0x", 2) || !strncmp(buf, "0X", 2))
    {
        base = 16;
        ptr = buf + 2;
    }

    // ���õײ�ת������
    ret = cmd_data_strtoul((const char*)ptr, 0, base);

    return ret;
}

/**
 * @brief  �ַ���ת�з��ų����ͣ�֧�������� + �Զ����ƣ�
 * @param  cp:    �����ַ���
 * @param  endp:  ��������λ��
 * @param  base:  ����
 * @return �з��ų�����
 */
static long cmd_data_strtol(const char* cp, char** endp, unsigned int base)
{
    long result = 0;
    int sign = 1;       // ���ţ�1=����-1=��
    int value;

    // ������ͷ�հ׷�
    while (isspace((unsigned char)*cp))
    {
        cp++;
    }

    // ����������
    if (*cp == '-')
    {
        sign = -1;
        cp++;
    }
    else if (*cp == '+')
    {
        cp++;
    }

    // ���� 0 ��ͷ���˽��� / ʮ�����ƣ�
    if (*cp == '0')
    {
        cp++;

        // 0x / 0X �� 16 ����
        if ((*cp == 'x' || *cp == 'X') && isxdigit((unsigned char)cp[1]))
        {
            base = 16;
            cp++;
        }
        // δָ������ �� Ĭ�� 8 ����
        else if (!base)
        {
            base = 8;
        }
    }

    // δָ������ �� Ĭ�� 10 ����
    if (!base)
    {
        base = 10;
    }

    // ���ַ�ת��
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

        // �������Ʒ�Χ �� ֹͣ����
        if (value >= (int)base)
        {
            break;
        }

        result = result * base + value;
        cp++;
    }

    // ��¼����λ��
    if (endp)
    {
        *endp = (char*)cp;
    }

    // ���Ϸ��ŷ���
    return sign * result;
}

/**
 * @brief  ����ӿڣ��ַ���ת�з��ų�����
 * @param  buf: �����ַ�����֧�� +123 / -456 / 0xAB��
 * @return �з��ų�����
 */
signed long get_arg_long(char* buf)
{
    long ret = 0;
    int base = 10;
    char *ptr = buf;
    int sign = 1;

    // �����հ�
    while (isspace((unsigned char)*ptr))
    {
        ptr++;
    }

    // ʶ�����
    if (*ptr == '-')
    {
        sign = -1;
        ptr++;
    }
    else if (*ptr == '+')
    {
        ptr++;
    }

    // ʶ�� 0x ʮ������
    if (!strncasecmp(ptr, "0x", 2))
    {
        base = 16;
        ptr += 2;
    }

    // ���ý�������
    ret = cmd_data_strtol(ptr, 0, base);

    // Ӧ�÷���
    if (sign < 0 && ret > 0)
    {
        ret = -ret;
    }

    return ret;
}

