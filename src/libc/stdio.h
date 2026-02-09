#ifndef LIBC_STDIO_H
#define LIBC_STDIO_H

#include "stdint.h"
#include "stdarg.h"

void itoa(int n, char* s, int base);
int  vsnprintf(char* str, size_t size, const char* format, va_list ap);
int  sprintf(char* str, const char* format, ...);

#endif
