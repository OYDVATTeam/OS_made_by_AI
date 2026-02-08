#include "vga.h"   // Handles pixels, rects, windows, and cursor
#include "mouse.h" // Handles mouse_init, mouse_poll, etc.
#include "font.h"  // Handles draw_string, draw_char

unsigned char saved_pixels[4];

void hide_mouse(int x, int y) {
    if (x < 0 || x >= SCREEN_WIDTH - 1 || y < 0 || y >= SCREEN_HEIGHT - 1) return;
    put_pixel(x, y,         saved_pixels[0]);
    put_pixel(x + 1, y,     saved_pixels[1]);
    put_pixel(x, y + 1,     saved_pixels[2]);
    put_pixel(x + 1, y + 1, saved_pixels[3]);
}

void show_mouse(int x, int y) {
    if (x < 0 || x >= SCREEN_WIDTH - 1 || y < 0 || y >= SCREEN_HEIGHT - 1) return;
    saved_pixels[0] = get_pixel(x, y);
    saved_pixels[1] = get_pixel(x + 1, y);
    saved_pixels[2] = get_pixel(x, y + 1);
    saved_pixels[3] = get_pixel(x + 1, y + 1);
    draw_mouse_cursor(x, y);
}

void draw_number(int x, int y, int num, unsigned char color) {
    char buf[6];
    int i = 0;
    if (num == 0) buf[i++] = '0';
    else {
        while (num > 0 && i < 5) {
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
    for (i = 0; i < count; i++) { asm volatile ("nop"); }
}

void kernel_main(void) {
    fill_screen(COLOR_CYAN);
    draw_window(80, 50, 160, 100, "System Window");
    draw_string(10, 10, "POS:", COLOR_WHITE);

    mouse_init();

    int anim_x = 90;
    int direction = 1;
    int last_mx = 0, last_my = 0;

    show_mouse(last_mx, last_my);

    while (1) {
        hide_mouse(last_mx, last_my);

        // GUI Animation
        draw_rect(anim_x, 130, 20, 10, COLOR_GRAY);
        anim_x += direction;
        if (anim_x > 210 || anim_x < 90) direction *= -1;
        draw_rect(anim_x, 130, 20, 10, COLOR_WHITE);

        // Mouse Handling
        mouse_poll();
        int mx = mouse_get_x();
        int my = mouse_get_y();

        // Update Text
        draw_rect(45, 10, 80, 8, COLOR_CYAN);
        draw_number(45, 10, mx, COLOR_WHITE);
        draw_char(75, 10, ',', COLOR_WHITE);
        draw_number(85, 10, my, COLOR_WHITE);

        show_mouse(mx, my);
        last_mx = mx; last_my = my;

        delay(50000);
    }
}
