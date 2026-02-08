// kernel.c
#include "vga.h"
#include "mouse.h"

/* Forward declaration from gui.c */
void draw_window(int x, int y, int w, int h, const char* title);
void draw_mouse_cursor(int x, int y);

/* Simple busy-wait delay */
static void delay(int count) {
    volatile int i;
    for (i = 0; i < count; i++) {
        asm volatile ("nop");
    }
}

void kernel_main(void) {
    /* Clear screen */
    fill_screen(COLOR_CYAN);

    /* Draw a window */
    draw_window(80, 50, 160, 100, "System");

    /* Initialize mouse */
    mouse_init();

    int anim_x = 90;
    int direction = 1;

    int last_mouse_x = -1;
    int last_mouse_y = -1;

    while (1) {
        /* ----------------------------
           GUI animation
        -----------------------------*/

        /* Erase previous bar */
        draw_rect(anim_x, 120, 20, 10, COLOR_GRAY);

        /* Move bar */
        anim_x += direction;
        if (anim_x > 210 || anim_x < 90) {
            direction *= -1;
        }

        /* Draw bar */
        draw_rect(anim_x, 120, 20, 10, COLOR_WHITE);

        /* ----------------------------
           Mouse handling
        -----------------------------*/

        mouse_poll();

        int mx = mouse_get_x();
        int my = mouse_get_y();

        /* Erase old cursor */
        if (last_mouse_x >= 0 && last_mouse_y >= 0) {
            put_pixel(last_mouse_x, last_mouse_y, COLOR_GRAY);
            put_pixel(last_mouse_x + 1, last_mouse_y, COLOR_GRAY);
            put_pixel(last_mouse_x, last_mouse_y + 1, COLOR_GRAY);
        }

        /* Draw new cursor */
        draw_mouse_cursor(mx, my);

        last_mouse_x = mx;
        last_mouse_y = my;

        delay(80000);
    }
}
