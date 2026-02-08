// kernel.c
#include "vga.h"

// Forward declarations of GUI helpers
void draw_window(int x, int y, int w, int h, const char* title);

static void delay(int count) {
    volatile int i;
    for (i = 0; i < count; i++) {
        asm volatile("nop");
    }
}

void kernel_main(void) {
    // 1. Fill the screen with Cyan
    fill_screen(COLOR_CYAN);

    // 2. Draw a "GUI Window" in the center
    draw_window(80, 50, 160, 100, "System");

    int anim_x = 90;
    int direction = 1;

    while (1) {
        // Erase old frame (draw over with window body color)
        draw_rect(anim_x, 120, 20, 10, COLOR_GRAY);

        // Update position
        anim_x += direction;
        if (anim_x > 210 || anim_x < 90) {
            direction *= -1;
        }

        // Draw new frame (a small white bar moving inside the window)
        draw_rect(anim_x, 120, 20, 10, COLOR_WHITE);

        delay(100000);
    }
}
