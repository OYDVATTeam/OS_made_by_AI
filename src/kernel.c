#include "vga.h"
#include "mouse.h"
#include "font.h"

// Simple delay function to prevent the CPU from eating 100% in QEMU
void delay(int count) {
    volatile int i;
    for (i = 0; i < count; i++) {
        __asm__("nop");
    }
}

void kernel_main(void) {
    fill_screen(COLOR_CYAN);
    draw_taskbar();
    draw_icon_trash(20, 20);
    draw_window(80, 50, 160, 100, "System Window");
    
    mouse_init();
    
    int menu_open = 0;
    int last_click_state = 0;
    int mx, my, last_mx = 160, last_my = 100;

    while (1) {
        hide_mouse(last_mx, last_my);
        
        mouse_poll();
        mx = mouse_get_x();
        my = mouse_get_y();
        int clicked = mouse_get_button(); // Assuming 1 = Left Click

        // --- START MENU LOGIC ---
        // Start Button bounds: X(2-44), Y(186-198)
        if (clicked && !last_click_state) { // Click detected (rising edge)
            if (mx >= 2 && mx <= 44 && my >= 186 && my <= 198) {
                menu_open = !menu_open; // Toggle menu
                
                // Redraw background to "close" menu if needed
                if (!menu_open) {
                    fill_screen(COLOR_CYAN);
                    draw_taskbar();
                    draw_icon_trash(20, 20);
                    draw_window(80, 50, 160, 100, "System Window");
                }
            }
        }
        last_click_state = clicked;

        // --- DRAWING ---
        if (menu_open) {
            draw_start_menu();
        }

        // Display coordinates for positioning
        draw_rect(240, 5, 80, 10, COLOR_CYAN);
        draw_number(250, 5, mx, COLOR_WHITE);
        draw_number(290, 5, my, COLOR_WHITE);

        show_mouse(mx, my);
        last_mx = mx; last_my = my;
        delay(50000);
    }
}
