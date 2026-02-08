// kernel.c
#include "vga.h"
#include "mouse.h"
#include "font.h"

/* * External declarations. 
 * We don't define these here; we just tell the compiler 
 * they live in another file (gui.c).
 */
extern void draw_window(int x, int y, int w, int h, const char* title);
extern void draw_mouse_cursor(int x, int y);

/* Utility to draw a simple integer for coordinates */
void draw_number(int x, int y, int num, unsigned char color) {
    char buf[5];
    int i = 0;
    if (num == 0) { 
        buf[i++] = '0'; 
    } else {
        while (num > 0 && i < 4) {
            buf[i++] = (num % 10) + '0';
            num /= 10;
        }
    }
    while (i > 0) {
        draw_char(x, y, buf[--i], color);
        x += 8;
    }
}

static void delay(int count) {
    volatile int i;
    for (i = 0; i < count; i++) {
        asm volatile ("nop");
    }
}

void kernel_main(void) {
    /* 1. Initial Setup */
    fill_screen(COLOR_CYAN);
    draw_window(80, 50, 160, 100, "System");
    
    draw_string(90, 70, "AI OS Loaded", COLOR_BLACK);
    draw_string(10, 10, "Mouse X/Y:", COLOR_WHITE);

    mouse_init();

    int anim_x = 90;
    int direction = 1;
    int last_mouse_x = -1;
    int last_mouse_y = -1;

    while (1) {
        /* --- 2. GUI Animation --- */
        draw_rect(anim_x, 120, 20, 10, COLOR_GRAY); // Erase with window body color
        anim_x += direction;
        if (anim_x > 210 || anim_x < 90) direction *= -1;
        draw_rect(anim_x, 120, 20, 10, COLOR_WHITE); // Draw moving bar

        /* --- 3. Mouse Handling --- */
        mouse_poll();
        int mx = mouse_get_x();
        int my = mouse_get_y();

        /* Erase old cursor */
        if (last_mouse_x >= 0 && last_mouse_y >= 0) {
            unsigned char bg = COLOR_CYAN;
            // Check if last position was inside the window (80,50 to 240,150)
            if (last_mouse_x >= 80 && last_mouse_x <= 240 && 
                last_mouse_y >= 50 && last_mouse_y <= 150) {
                bg = COLOR_GRAY;
            }
            // Using 2x2 erase to match the 2x2 cursor
            draw_rect(last_mouse_x, last_mouse_y, 2, 2, bg);
        }

        /* Update Coordinates Text */
        draw_rect(90, 10, 40, 8, COLOR_CYAN); // Clear text area
        draw_number(90, 10, mx, COLOR_WHITE);
        draw_number(120, 10, my, COLOR_WHITE);

        /* Draw new cursor */
        draw_mouse_cursor(mx, my);

        last_mouse_x = mx;
        last_mouse_y = my;

        delay(60000);
    }
}
