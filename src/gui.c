// gui.c
#include "vga.h"
#include "font.h"

void fill_screen(unsigned char color) {
    unsigned char *vram = (unsigned char *)VRAM_ADDRESS;
    for (int i = 0; i < SCREEN_WIDTH * SCREEN_HEIGHT; i++) {
        vram[i] = color;
    }
}

void draw_rect(int x, int y, int w, int h, unsigned char color) {
    for (int i = 0; i < h; i++) {
        for (int j = 0; j < w; j++) {
            put_pixel(x + j, y + i, color);
        }
    }
}

void draw_window(int x, int y, int w, int h, const char* title) {
    // Window Shadow
    draw_rect(x + 3, y + 3, w, h, COLOR_BLACK);
    // Window Body
    draw_rect(x, y, w, h, COLOR_GRAY);
    // Title Bar
    draw_rect(x, y, w, 12, COLOR_BLUE);
    // Close Button (Little red square)
    draw_rect(x + w - 10, y + 2, 8, 8, COLOR_RED);

    draw_string(x + 4, y + 2, title, COLOR_WHITE);
}

void draw_mouse_cursor(int x, int y) {
    put_pixel(x, y, COLOR_WHITE);
    put_pixel(x + 1, y, COLOR_WHITE);
    put_pixel(x, y + 1, COLOR_WHITE);
    put_pixel(x + 1, y + 1, COLOR_WHITE);
}
