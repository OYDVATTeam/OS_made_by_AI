#include "stdlib.h"
#include "ctype.h"

int abs(int j) {
    return (j < 0) ? -j : j;
}

int atoi(const char* s) {
    int res = 0;
    int sign = 1;

    // Skip whitespace
    while (isspace(*s)) s++;

    // Check sign
    if (*s == '-') {
        sign = -1;
        s++;
    } else if (*s == '+') {
        s++;
    }

    // Convert digits
    while (isdigit(*s)) {
        res = res * 10 + (*s - '0');
        s++;
    }

    return sign * res;
}
