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
    // 1. Initialize Graphics
    fill_screen(COLOR_CYAN);
    
    // 2. Draw Desktop Environment
    draw_taskbar();
    draw_icon_trash(20, 20);
    draw_window(80, 50, 160, 100, "System Window");
    
    // 3. Setup Mouse
    mouse_init();
    
    int last_mx = 160, last_my = 100;
    int mx = 160, my = 100;

    // Initial draw of the cursor
    show_mouse(mx, my);

    while (1) {
        // --- PREPARE ---
        // Hide the mouse at its old position to restore the background
        hide_mouse(last_mx, last_my);

        // --- UPDATE ---
        // Poll the hardware for new mouse data
        mouse_poll();
        mx = mouse_get_x();
        my = mouse_get_y();

        // --- UI OVERLAYS ---
        // Display Mouse Coordinates at (250, 5) for debugging/positioning
        // We draw a small cyan box first to clear old numbers
        draw_rect(240, 5, 75, 10, COLOR_CYAN);
        
        // Custom draw_number doesn't exist yet? Use draw_char/string for now:
        draw_string(240, 5, "X:", COLOR_WHITE);
        // Note: If you don't have a 'draw_number' function yet, 
        // we can just stick to the labels for now.
        
        // --- DRAW ---
        // Show the mouse at the new position
        show_mouse(mx, my);

        // Store current position for the next frame's 'hide_mouse'
        last_mx = mx;
        last_my = my;

        // Small delay to keep things smooth
        delay(50000);
    }
}
