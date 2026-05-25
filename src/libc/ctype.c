// making a c library from scratch? pfft, people would rather create hello world that recreate something like glibc
// sadly, there is no return, i am making the c library

#include "ctype.h"

int isspace(int c) {
    return (c == ' ' || c == '\t' || c == '\n' || c == '\v' || c == '\f' || c == '\r');
}

int isdigit(int c) {
    return (c >= '0' && c <= '9');
}

int isupper(int c) {
    return (c >= 'A' && c <= 'Z');
}

int islower(int c) {
    return (c >= 'a' && c <= 'z');
}

int toupper(int c) {
    if (islower(c)) return c - 0x20;
    return c;
}

int tolower(int c) {
    if (isupper(c)) return c + 0x20;
    return c;
}
