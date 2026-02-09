#include "vga.h"
#include "mouse.h"

// Simple loop to slow down the CPU for smoother UI movement
void delay(int count) {
    volatile int i;
    for (i = 0; i < count; i++) { __asm__("nop"); }
}

// Function to draw the base UI layers
void draw_desktop() {
    fill_screen(COLOR_CYAN);
    draw_taskbar();
    draw_icon_trash(20, 20);
    draw_window(80, 50, 160, 100, "System Window");
}

void kernel_main(void) {
    // 1. Hardware Initialization
    mouse_init();
    
    // 2. Initial Draw
    draw_desktop();

    int menu_open = 0;
    int last_click_state = 0;
    int mx = 160, my = 100;
    int last_mx = 160, last_my = 100;

    while (1) {
        // --- STEP 1: RESTORE BACKGROUND ---
        // Hide mouse cursor at old position before anything else
        hide_mouse(last_mx, last_my);

        // --- STEP 2: POLL HARDWARE ---
        mouse_poll();
        mx = mouse_get_x();
        my = mouse_get_y();
        int clicked = mouse_get_button();

        // --- STEP 3: INTERACTION LOGIC ---
        // Check for "Rising Edge" click (button just went down)
        if (clicked && !last_click_state) {
            // Start Button coordinates check
            if (mx >= 2 && mx <= 44 && my >= 186 && my <= 198) {
                menu_open = !menu_open; // Toggle the menu state
                
                // If closing, we must redraw the desktop to clear the menu pixels
                if (!menu_open) {
                    draw_desktop();
                }
            }
        }
        last_click_state = clicked;

        // --- STEP 4: DRAWING OVERLAYS ---
        // Draw the menu if it's currently open
        if (menu_open) {
            draw_start_menu();
        }

        // Live Coordinate Debugger (Top Right)
        draw_rect(240, 5, 80, 10, COLOR_CYAN);
        draw_number(250, 5, mx, COLOR_WHITE);
        draw_number(290, 5, my, COLOR_WHITE);

        // Finally, draw the mouse cursor at the new position
        show_mouse(mx, my);

        // Update tracking variables for the next frame
        last_mx = mx;
        last_my = my;

        delay(40000); // Maintain a stable frame rate
    }
}
