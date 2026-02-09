#ifndef VGA_H
#define VGA_H

#define SCREEN_WIDTH  320
#define SCREEN_HEIGHT 200
#define VRAM_ADDRESS  0xA0000

#define COLOR_BLACK   0x00
#define COLOR_BLUE    0x01
#define COLOR_GREEN   0x02
#define COLOR_CYAN    0x03
#define COLOR_RED     0x04
#define COLOR_MAGENTA 0x05
#define COLOR_BROWN   0x06
#define COLOR_LIGHT_GRAY 0x07
#define COLOR_GRAY    0x08
#define COLOR_DARK_GRAY 8
#define COLOR_LIGHT_BLUE 0x09
#define COLOR_LIGHT_GREEN 0x0A
#define COLOR_LIGHT_CYAN 0x0B
#define COLOR_LIGHT_RED 0x0C
#define COLOR_LIGHT_MAGENTA 0x0D
#define COLOR_YELLOW  0x0E  // The one that was missing!
#define COLOR_WHITE   0x0F

// Inlined Pixel Functions
static inline void put_pixel(int x, int y, unsigned char color) {
    if (x < 0 || x >= SCREEN_WIDTH || y < 0 || y >= SCREEN_HEIGHT) return;
    vram[y * SCREEN_WIDTH + x] = color;
}

static inline unsigned char get_pixel(int x, int y) {
    if (x < 0 || x >= SCREEN_WIDTH || y < 0 || y >= SCREEN_HEIGHT) return 0;
    unsigned char *vram = (unsigned char *)VRAM_ADDRESS;
    return vram[y * SCREEN_WIDTH + x];
}

/* --- PROTOTYPES --- */
void vga_init(); // Added: Needed by kernel.c
void fill_screen(unsigned char color);
void fill_rect(int x, int y, int w, int h, unsigned char color);     // Added: Was missing!
void draw_rect(int x, int y, int w, int h, unsigned char color);
void draw_string(int x, int y, const char* str, unsigned char color); // Added: Was missing!
void draw_window(int x, int y, int w, int h, const char* title);
void draw_mouse_cursor(int x, int y);
void draw_number(int x, int y, int num, unsigned char color);
void draw_taskbar();
void draw_icon_trash(int x, int y);
void draw_start_menu();

#endif
