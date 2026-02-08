// src/font.h
#ifndef FONT_H
#define FONT_H

#include "vga.h"

// 8x8 Bitmap Font (Basic ASCII 0-127)
extern const unsigned char font_bitmap[128][8];

void draw_char(int x, int y, char c, unsigned char color);
void draw_string(int x, int y, const char* str, unsigned char color);

#endif
