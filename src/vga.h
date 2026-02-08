// vga.h
#ifndef VGA_H
#define VGA_H

#define SCREEN_WIDTH  320
#define SCREEN_HEIGHT 200
#define VRAM_ADDRESS  0xA0000

// Default VGA 256-color palette indices
#define COLOR_BLACK  0
#define COLOR_BLUE   1
#define COLOR_CYAN   3
#define COLOR_GRAY   7
#define COLOR_DARK_GRAY 8
#define COLOR_WHITE  15
#define COLOR_RED    4

void put_pixel(int x, int y, unsigned char color);
void fill_screen(unsigned char color);
void draw_rect(int x, int y, int w, int h, unsigned char color);

#endif
