#ifndef LIBC_STDLIB_H
#define LIBC_STDLIB_H

#include "stdint.h"

int  abs(int j);
int  atoi(const char* s);

// Note: malloc/free require a memory manager/heap which 
// we haven't built yet, so they are omitted for now.

#endif
