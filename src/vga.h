#ifndef VGA_H
#define VGA_H

#define SCREEN_WIDTH  320
#define SCREEN_HEIGHT 200
#define VRAM_ADDRESS  0xA0000

// Palette
#define COLOR_BLACK     0
#define COLOR_BLUE      1
#define COLOR_CYAN      3
#define COLOR_RED       4
#define COLOR_GRAY      7
#define COLOR_DARK_GRAY 8
#define COLOR_WHITE     15

// Inlined Pixel Functions
static inline void put_pixel(int x, int y, unsigned char color) {
    if (x < 0 || x >= SCREEN_WIDTH || y < 0 || y >= SCREEN_HEIGHT) return;
    unsigned char *vram = (unsigned char *)VRAM_ADDRESS;
    vram[y * SCREEN_WIDTH + x] = color;
}

static inline unsigned char get_pixel(int x, int y) {
    if (x < 0 || x >= SCREEN_WIDTH || y < 0 || y >= SCREEN_HEIGHT) return 0;
    unsigned char *vram = (unsigned char *)VRAM_ADDRESS;
    return vram[y * SCREEN_WIDTH + x];
}

/* --- ADD THESE PROTOTYPES BELOW --- */
void fill_screen(unsigned char color);
void draw_rect(int x, int y, int w, int h, unsigned char color);
void draw_window(int x, int y, int w, int h, const char* title);
void draw_mouse_cursor(int x, int y);

#endif
