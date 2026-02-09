// gui.c
#include "vga.h"
#include "font.h"

unsigned char* vram;

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

void draw_taskbar() {
    // Background bar
    draw_rect(0, 184, 320, 16, COLOR_GRAY);
    // 3D Top Border
    draw_rect(0, 184, 320, 1, COLOR_WHITE);
    // Start Button
    draw_rect(2, 186, 42, 12, COLOR_BLUE);
    draw_string(5, 188, "START", COLOR_WHITE);
    // Clock Area
    draw_rect(270, 186, 48, 12, COLOR_DARK_GRAY);
    draw_string(275, 188, "12:00", COLOR_WHITE);
}

void draw_icon_trash(int x, int y) {
    // The Bin Body
    draw_rect(x + 4, y + 6, 12, 14, COLOR_WHITE);    
    draw_rect(x + 5, y + 7, 10, 12, COLOR_DARK_GRAY); 
    
    // Vertical details (the "ribs" of the bin)
    put_pixel(x + 7, y + 9, COLOR_GRAY);
    put_pixel(x + 7, y + 11, COLOR_GRAY);
    put_pixel(x + 10, y + 9, COLOR_GRAY);
    put_pixel(x + 10, y + 11, COLOR_GRAY);

    // The Lid
    draw_rect(x + 2, y + 3, 16, 2, COLOR_GRAY);      
    draw_rect(x + 7, y + 1, 6, 2, COLOR_GRAY);       

    // Text Label
    draw_string(x - 8, y + 22, "Trash Can", COLOR_WHITE);
}

void draw_start_menu() {
    // 1. Shadow (for that 3D look)
    draw_rect(5, 105, 80, 80, COLOR_BLACK);
    // 2. Menu Body (Gray rectangle)
    draw_rect(2, 102, 80, 80, COLOR_GRAY);
    // 3. Side strip (The classic dark blue vertical bar on the left)
    draw_rect(2, 102, 15, 80, COLOR_BLUE);
    
    // 4. Menu Items
    draw_string(20, 110, "Programs", COLOR_WHITE);
    draw_string(20, 130, "Settings", COLOR_WHITE);
    draw_string(20, 150, "Files", COLOR_WHITE);
    
    // 5. Divider Line
    draw_rect(18, 165, 60, 1, COLOR_DARK_GRAY);
    
    // 6. Shutdown
    draw_string(20, 170, "Shutdown", COLOR_WHITE);
}

void draw_number(int x, int y, int num, unsigned char color) {
    char buf[10];
    int i = 0;
    if (num == 0) {
        draw_char(x, y, '0', color);
        return;
    }
    while (num > 0 && i < 10) {
        buf[i++] = (num % 10) + '0';
        num /= 10;
    }
    for (int j = 0; j < i; j++) {
        draw_char(x + (i - 1 - j) * 8, y, buf[j], color);
    }
}

// 1. Add this! The kernel calls it to initialize video.
void vga_init() {
    // We are already in Mode 13h, so we just clear the screen to start fresh.
    fill_screen(COLOR_BLACK);
}

// 2. Add this! It's just a wrapper or rename of your draw_rect.
// The linker is specifically looking for "fill_rect".
void fill_rect(int x, int y, int w, int h, unsigned char color) {
    unsigned char *vram = (unsigned char *)0xA0000;
    for (int i = 0; i < h; i++) {
        for (int j = 0; j < w; j++) {
            // Manual bounds check
            int cur_x = x + j;
            int cur_y = y + i;
            if(cur_x >= 0 && cur_x < 320 && cur_y >= 0 && cur_y < 200) {
                vram[cur_y * 320 + cur_x] = color;
            }
        }
    }
}
