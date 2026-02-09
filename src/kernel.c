#include "libc/stdint.h"
#include "libc/stdio.h"
#include "vga.h"
#include "mouse.h"
#include "fat.h"
#include "notepad.h"

// Screen dimensions (assuming 320x200 VGA Mode 13h)
#define SCREEN_WIDTH  320
#define SCREEN_HEIGHT 200

// UI Component positions
#define TASKBAR_HEIGHT 14
#define START_BTN_W    45
#define MENU_W         80
#define MENU_H         50

// Global UI State
static int start_menu_open = 0;

/**
 * handle_input
 * Processes mouse coordinates and clicks to drive the UI logic.
 */
void handle_input(int mx, int my, int clicked) {
    static int last_clicked = 0;

    // Detect "Mouse Down" event
    if (clicked && !last_clicked) {
        
        // 1. Logic for the Start Button (Bottom-Left)
        if (mx >= 0 && mx <= START_BTN_W && my >= (SCREEN_HEIGHT - TASKBAR_HEIGHT)) {
            start_menu_open = !start_menu_open; // Toggle menu
        } 
        
        // 2. Logic when the Start Menu is actually open
        else if (start_menu_open) {
            // Check if clicking "Notepad" entry inside the menu box
            // Assuming Notepad is the first item in the menu list
            if (mx >= 0 && mx <= MENU_W && my >= (SCREEN_HEIGHT - TASKBAR_HEIGHT - 20) && my < (SCREEN_HEIGHT - TASKBAR_HEIGHT)) {
                notepad_open("README.TXT");
                start_menu_open = 0; // Auto-close menu
            } else {
                // Clicked outside menu items? Close it.
                start_menu_open = 0;
            }
        }
        
        // 3. Logic for closing the Notepad (Clicking the 'X' area)
        // Adjust these coords based on where notepad_draw() puts the [X]
        if (mx >= 240 && mx <= 260 && my >= 50 && my <= 65) {
            notepad_close();
        }
    }
    
    last_clicked = clicked;
}

/**
 * render_desktop
 * Draws the background, taskbar, and start menu.
 */
void render_desktop(int mx, int my) {
    // 1. Draw Background (Cyan Desktop)
    fill_screen(COLOR_LIGHT_CYAN);

    // 2. Draw Taskbar
    fill_rect(0, SCREEN_HEIGHT - TASKBAR_HEIGHT, SCREEN_WIDTH, TASKBAR_HEIGHT, COLOR_GRAY);
    
    // 3. Draw Start Button
    uint8_t start_color = (mx < START_BTN_W && my > SCREEN_HEIGHT - TASKBAR_HEIGHT) ? COLOR_WHITE : COLOR_LIGHT_GRAY;
    fill_rect(0, SCREEN_HEIGHT - TASKBAR_HEIGHT, START_BTN_W, TASKBAR_HEIGHT, start_color);
    draw_string(5, SCREEN_HEIGHT - 11, "START", COLOR_BLACK);

    // 4. Draw Start Menu (above taskbar)
    if (start_menu_open) {
        int menu_y = SCREEN_HEIGHT - TASKBAR_HEIGHT - MENU_H;
        fill_rect(0, menu_y, MENU_W, MENU_H, COLOR_LIGHT_GRAY);
        draw_rect(0, menu_y, MENU_W, MENU_H, COLOR_BLACK); // Border

        // "Notepad" Entry with hover effect
        if (mx >= 0 && mx <= MENU_W && my >= (SCREEN_HEIGHT - TASKBAR_HEIGHT - 20) && my < (SCREEN_HEIGHT - TASKBAR_HEIGHT)) {
            fill_rect(1, SCREEN_HEIGHT - TASKBAR_HEIGHT - 18, MENU_W - 2, 16, COLOR_BLUE);
            draw_string(8, SCREEN_HEIGHT - TASKBAR_HEIGHT - 14, "Notepad", COLOR_WHITE);
        } else {
            draw_string(8, SCREEN_HEIGHT - TASKBAR_HEIGHT - 14, "Notepad", COLOR_BLACK);
        }
        
        // "System" Entry (Disabled)
        draw_string(8, menu_y + 5, "Settings", COLOR_GRAY);
    }
}

/**
 * kernel_main
 * The entry point of your OS.
 */
void kernel_main() {
    // Initialize Hardware
    vga_init();
    mouse_init();
    fat_init();

    // Loop Variables
    int mx = 160, my = 100, m_btn = 0;

    // Optional: Initial Splash or Startup Check
    // notepad_open("BOOT.LOG"); 

    while (1) {
        // 1. Get Mouse Data
        mouse_get_state(&mx, &my, &m_btn);

        // 2. Process Interaction
        handle_input(mx, my, m_btn);

        // 3. Start Rendering (Order matters!)
        render_desktop(mx, my);     // Layer 0: Background/Taskbar
        
        notepad_draw();             // Layer 1: Application Windows
        
        draw_mouse_cursor(mx, my);  // Layer 2: Mouse (Always on top)

        // 4. Double Buffering / VSync
        // If you implemented a backbuffer, swap it here.
        // wait_vsync(); 
    }
}
