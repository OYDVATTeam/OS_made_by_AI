#include "stdio.h"
#include "string.h"
#include "stdarg.h"

// Helper: Integer to ASCII
void itoa(int n, char* s, int base) {
    int i = 0;
    int sign = n;
    if (sign < 0 && base == 10) n = -n;

    do {
        int digit = n % base;
        s[i++] = (digit > 9) ? (digit - 10) + 'A' : digit + '0';
    } while ((n /= base) > 0);

    if (sign < 0 && base == 10) s[i++] = '-';
    s[i] = '\0';

    // Reverse
    for (int j = 0, k = i - 1; j < k; j++, k--) {
        char temp = s[j];
        s[j] = s[k];
        s[k] = temp;
    }
}

// Helper: Formats string into a buffer
int vsnprintf(char* str, size_t size, const char* format, va_list ap) {
    size_t i = 0;
    const char* f = format;

    while (*f && i < size - 1) {
        if (*f == '%') {
            f++;
            if (*f == 'd') {
                char num[32];
                itoa(va_arg(ap, int), num, 10);
                size_t len = strlen(num);
                for (size_t j = 0; j < len && i < size - 1; j++) str[i++] = num[j];
            } else if (*f == 'x') {
                char num[32];
                itoa(va_arg(ap, int), num, 16);
                size_t len = strlen(num);
                for (size_t j = 0; j < len && i < size - 1; j++) str[i++] = num[j];
            } else if (*f == 's') {
                char* s_arg = va_arg(ap, char*);
                while (*s_arg && i < size - 1) str[i++] = *s_arg++;
            } else if (*f == 'c') {
                char c_arg = (char)va_arg(ap, int); // char promotes to int in varargs
                if (i < size - 1) str[i++] = c_arg;
            }
        } else {
            str[i++] = *f;
        }
        f++;
    }
    str[i] = '\0';
    return i;
}

// Public: printf to string
int sprintf(char* str, const char* format, ...) {
    va_list args;
    va_start(args, format);
    // We use a large size limit since we don't know the buffer size here
    int ret = vsnprintf(str, 65535, format, args);
    va_end(args);
    return ret;
}
