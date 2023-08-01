#include "tools/klib.h"
#include "comm/cpu_instr.h"
#include "tools/log.h"

void kernel_strcpy(char* dest, char* src)
{
    if (!dest || !src) {
        return;
    }

    while (*dest && *src) {
        *dest++ = *src++;
    }
    *dest = '\0';
}

void kernel_strncpy(char* dest, const char* src, int size)
{
    if (!dest || !src || !size) {
        return;
    }

    char* d = dest;
    const char* s = src;

    while ((size-- > 0) && (*s)) {
        *d++ = *s++;
    }
    if (size == 0) {
        *(d - 1) = '\0';
    } else {
        *d = '\0';
    }
}

int kernel_strlen(const char* str)
{
    if (str == (const char*)0) {
        return 0;
    }

    const char* c = str;

    int len = 0;
    while (*c++) {
        len++;
    }

    return len;
}

/**
 * 比较两个字符串，最多比较size个字符
 * 如果某一字符串提前比较完成，也算相同
 */
int kernel_strncmp(const char* s1, const char* s2, int size)
{
    if (!s1 || !s2) {
        return -1;
    }

    // 这里size没有用到
    while (*s1 && *s2 && (*s1 == *s2) && size) {
        s1++;
        s2++;
        size--;
    }

    return !((*s1 == '\0') || (*s2 == '\0') || (*s1 == *s2));
}

void kernel_memcpy(void* dest, void* src, int size)
{
    if (!dest || !src || !size) {
        return;
    }

    uint8_t* s = (uint8_t*)src;
    uint8_t* d = (uint8_t*)dest;
    while (size--) {
        *d++ = *s++;
    }
}

void kernel_memset(void* dest, uint8_t v, int size)
{
    if (!dest || !size) {
        return;
    }

    uint8_t* d = (uint8_t*)dest;
    while (size--) {
        *d++ = v;
    }
}

int kernel_memcmp(void* d1, void* d2, int size)
{
    if (!d1 || !d2) {
        return 1;
    }

    uint8_t* p_d1 = (uint8_t*)d1;
    uint8_t* p_d2 = (uint8_t*)d2;
    while (size--) {
        if (*p_d1++ != *p_d2++) {
            return 1;
        }
    }

    return 0;
}

void kernel_integer_to_str(char* str, int num, int x)
{
    // 转换字符索引[-15, -14, ...-1, 0, 1, ...., 14, 15]
    static const char* num2ch = { "FEDCBA9876543210123456789ABCDEF" };
    char* p = str;
    int old_num = num;

    // 仅支持部分进制
    if ((x != 2) && (x != 8) && (x != 10) && (x != 16)) {
        *p = '\0';
        return;
    }

    // 只支持十进制负数
    int signed_num = 0;
    if ((num < 0) && (x == 10)) {
        *p++ = '-';
        signed_num = 1;
    }

    if (signed_num) {
        do {
            char ch = num2ch[num % x + 15];
            *p++ = ch;
            num /= x;
        } while (num);
    } else {
        uint32_t u_num = (uint32_t)num;
        do {
            char ch = num2ch[u_num % x + 15];
            *p++ = ch;
            u_num /= x;
        } while (u_num);
    }
    *p-- = '\0';

    // 将转换结果逆序，生成最终的结果
    char* start = (!signed_num) ? str : str + 1;
    while (start < p) {
        char ch = *start;
        *start = *p;
        *p-- = ch;
        start++;
    }
}

void kernel_vsprintf(char* buf, const char* msg, va_list args)
{
    enum {
        NORMAL,
        READ_FMT
    } state
        = NORMAL;
    char* cur = buf;
    char ch;
    while (ch = *msg++) {
        switch (state) {
        case NORMAL:
            if (ch == '%') {
                state = READ_FMT;
            } else {
                *cur++ = ch;
            }
            break;
        case READ_FMT:
            if (ch == 's') {
                const char* str = va_arg(args, char*);
                int len = kernel_strlen(str);
                while (len--) {
                    *cur++ = *str++;
                }
            } else if (ch == 'c') {
                char c = va_arg(args, int);
                *cur++ = c;
            } else if (ch == 'd') {
                int num = va_arg(args, int);
                kernel_integer_to_str(cur, num, 10);
                cur += kernel_strlen(cur);
            } else if (ch == 'x') {
                int num = va_arg(args, int);
                kernel_integer_to_str(cur, num, 16);
                cur += kernel_strlen(cur);
            }
            state = NORMAL;
            break;
        }
    }
}

void pannic(const char* file, int line, const char* func, const char* cond)
{
    log_printf("assert failed! %s", cond);
    log_printf("file: %s\nline: %d, func: %s\n", file, line, func);
    while (1) {
        hlt();
    }
}

char* get_file_name(char* filename)
{
    char* p = filename;
    // 移动到末尾
    while (*p != '\0') {
        p++;
    }
    // 往前
    while ((p >= filename) && (*p != '/') && (*p != '\\')) {
        p--;
    }
    return p + 1;
}

int strings_count(char** filename)
{
    int cnt = 0;
    if (filename) {
        while (*filename++) {
            cnt++;
        }
    }
    return cnt;
}
