#ifndef KLIB_H
#define KLIB_H

#include "comm/types.h"
#include <stdarg.h>

void kernel_strcpy(char *dest, char *src);
void kernel_strncpy(char *dest, const char *src, int size);
int kernel_strncmp(const char *s1, const char *s2, int size);
int kernel_strlen(const char *str);

void kernel_memcpy(void *dest, void *src, int size);
void kernel_memset(void *dest, uint8_t v, int size);
int kernel_memcmp(void *d1, void *d2, int size);

void kernel_vsprintf(char *buf, const char *msg, va_list args);

#ifndef RELEASE

#define ASSERT(expr) \
    if (!(expr))       \
    pannic(__FILE__, __LINE__, __func__, #expr)

void pannic(const char *file, int line, const char *func, const char *cond);

#else
#define ASSERT(expr) (1 == 1)
#endif

#endif