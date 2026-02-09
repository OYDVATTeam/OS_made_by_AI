#include "mouse.h"
#include "io.h"  // Using your existing I/O header to avoid redefinition errors
#include "vga.h"

// PERSISTENT STATE - These keep the "Truth" of the hardware
static int global_mouse_x = 160;
static int global_mouse_y = 100;
static int last_button_state = 0;


// Persistent State
static int global_mouse_x = 160;
static int global_mouse_y = 100;
static int last_button_state = 0;

// This buffer saves the 4x4 pixels "under" the mouse so we can restore them later
static unsigned char mouse_back_buffer[16];

// Simple 4x4 pixel cursor drawing
void draw_mouse_cursor(int x, int y) {
    draw_rect(x, y, 4, 4, COLOR_WHITE);    // Main body
    put_pixel(x, y, COLOR_BLACK);          // Top-left pixel for "pointy" look
}

void show_mouse(int x, int y) {
    // 1. Save the pixels that are currently on the screen at this location
    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 4; j++) {
            mouse_back_buffer[i * 4 + j] = get_pixel(x + j, y + i);
        }
    }
    // 2. Draw the white cursor on top
    draw_mouse_cursor(x, y);
}

void hide_mouse(int x, int y) {
    // 1. Put the saved pixels back, erasing the mouse without destroying the UI
    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 4; j++) {
            put_pixel(x + j, y + i, mouse_back_buffer[i * 4 + j]);
        }
    }
}

// Helper to wait for the PS/2 Controller buffer
void mouse_wait(unsigned char type) {
    unsigned int timeout = 100000;
    if (type == 0) {
        while (timeout-- && !(inb(0x64) & 1)); // Wait for data to be ready to read
    } else {
        while (timeout-- && (inb(0x64) & 2)); // Wait for controller ready to receive
    }
}

// Write a command to the mouse via the controller
void mouse_write(unsigned char data) {
    mouse_wait(1);
    outb(0x64, 0xD4); // Signal we are sending a command to the mouse
    mouse_wait(1);
    outb(0x60, data);
}

void mouse_init() {
    mouse_wait(1);
    outb(0x64, 0xA8); // Enable the auxiliary mouse device
    
    mouse_write(0xF4); // Command 0xF4: Enable Data Reporting
    inb(0x60);         // Read the ACK (acknowledgment) byte to clear the buffer
}

// This function acts as the bridge between hardware and your kernel variables
void mouse_poll() {
    // If bit 0 of 0x64 is 0, the mouse has no new data to report
    if (!(inb(0x64) & 0x01)) return;

    // Read the 3-byte standard PS/2 packet
    unsigned char status = inb(0x60);
    char rel_x = (char)inb(0x60);
    char rel_y = (char)inb(0x60);

    // Update the button state (Bit 0 is Left Click)
    // This stays '1' as long as the hardware reports the button is held
    last_button_state = (status & 0x01);

    // Update coordinates
    global_mouse_x += rel_x;
    global_mouse_y -= rel_y; // PS/2 Y is inverted relative to VGA screen Y

    // Clamping to screen resolution (320x200)
    if (global_mouse_x < 0) global_mouse_x = 0;
    if (global_mouse_y < 0) global_mouse_y = 0;
    if (global_mouse_x > 315) global_mouse_x = 315;
    if (global_mouse_y > 195) global_mouse_y = 195;
}

// These functions allow the kernel to read the state updated by mouse_poll
int mouse_get_button() { return last_button_state; }
int mouse_get_x()      { return global_mouse_x; }
int mouse_get_y()      { return global_mouse_y; }
