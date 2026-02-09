#include "libc/stdint.h"
#include "vga.h"
#include "mouse.h"
#include "fat.h"
#include "notepad.h"

/**
 * Multiboot 1 Information Structure
 * We need this to find where GRUB actually put the video memory.
 */
struct multiboot_info {
    uint32_t flags;
    uint32_t unused[10];
    uint32_t mmap_length;
    uint32_t mmap_addr;
    uint32_t unused2[9];
    // Offset 88: Framebuffer Address
    uint64_t framebuffer_addr;
    uint32_t framebuffer_pitch;
    uint32_t framebuffer_width;
    uint32_t framebuffer_height;
    uint8_t  framebuffer_bpp;
    uint8_t  framebuffer_type;
} __attribute__((packed));

// Global UI State
static int start_menu_open = 0;
extern unsigned char* vram; // This matches the change in vga.h/gui.c

/**
 * handle_input
 * Manages the logic for clicks on the Start Button, Menu, and Notepad.
 */
void handle_input(int mx, int my, int clicked) {
    static int last_clicked = 0;

    if (clicked && !last_clicked) {
        // 1. Start Button Click
        if (mx >= 0 && mx <= 45 && my >= 186) {
            start_menu_open = !start_menu_open;
        } 
        // 2. Start Menu Logic
        else if (start_menu_open) {
            // Click Notepad (Based on draw_start_menu coordinates)
            if (mx >= 0 && mx <= 80 && my >= 102 && my <= 182) {
                // If clicking the middle section where "Programs/Files" are
                if (my > 120 && my < 140) {
                    notepad_open("README.TXT");
                    start_menu_open = 0;
                }
            } else {
                start_menu_open = 0;
            }
        }
        // 3. Notepad Close (X button area)
        if (mx >= 240 && mx <= 260 && my >= 50 && my <= 65) {
            notepad_close();
        }
    }
    last_clicked = clicked;
}

/**
 * render_ui
 * Draws everything in the correct z-order.
 */
void render_ui(int mx, int my) {
    // Layer 0: Desktop Background
    fill_screen(COLOR_LIGHT_CYAN);

    // Layer 1: Icons (Trash can, etc)
    draw_icon_trash(10, 10);

    // Layer 2: Taskbar
    draw_taskbar();

    // Layer 3: Start Menu (if open)
    if (start_menu_open) {
        draw_start_menu();
    }

    // Layer 4: Applications
    notepad_draw();

    // Layer 5: Mouse Cursor (Always Top)
    draw_mouse_cursor(mx, my);
}

/**
 * kernel_main
 * Entry point called from boot.asm
 */
void kernel_main(struct multiboot_info* mbi) {
    // 1. Fix the Glitch: Map VRAM to the address provided by GRUB
    // Bit 12 of flags indicates if framebuffer info is present
    if (mbi->flags & (1 << 12)) {
        vram = (unsigned char*)(uint32_t)mbi->framebuffer_addr;
    } else {
        vram = (unsigned char*)0xA0000; // Fallback
    }

    // 2. Initialize Hardware
    vga_init();
    mouse_init();
    fat_init();

    int mx = 160, my = 100, m_btn = 0;

    // 3. Main OS Loop
    while (1) {
        // IMPORTANT: Since interrupts are disabled (cli), 
        // we must poll the mouse hardware manually every frame.
        mouse_poll(); 
        mouse_get_state(&mx, &my, &m_btn);

        // Update Logic
        handle_input(mx, my, m_btn);

        // Draw Everything
        render_ui(mx, my);

        // Simple delay to prevent CPU from screaming
        for (volatile int i = 0; i < 10000; i++);
    }
}
