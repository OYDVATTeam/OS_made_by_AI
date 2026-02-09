#include "mouse.h"
#include "vga.h"
#include "io.h"

// 1. VARIABLE DEFINITIONS (Only once!)
static int global_mouse_x = 160;
static int global_mouse_y = 100;
static int last_button_state = 0;
static unsigned char mouse_back_buffer[16]; // Stores pixels under the 4x4 cursor

// 2. HARDWARE COMMUNICATION
void mouse_wait(unsigned char type) {
    unsigned int timeout = 100000;
    if (type == 0) {
        while (timeout-- && !(inb(0x64) & 1));
    } else {
        while (timeout-- && (inb(0x64) & 2));
    }
}

void mouse_write(unsigned char data) {
    mouse_wait(1);
    outb(0x64, 0xD4);
    mouse_wait(1);
    outb(0x60, data);
}

void mouse_init() {
    mouse_wait(1);
    outb(0x64, 0xA8); // Enable mouse
    mouse_write(0xF4); // Enable data reporting
    inb(0x60);         // Acknowledge
}

void mouse_poll() {
    if (!(inb(0x64) & 0x01)) return;

    unsigned char status = inb(0x60);
    char rel_x = (char)inb(0x60);
    char rel_y = (char)inb(0x60);

    last_button_state = (status & 0x01);
    global_mouse_x += rel_x;
    global_mouse_y -= rel_y;

    if (global_mouse_x < 0) global_mouse_x = 0;
    if (global_mouse_y < 0) global_mouse_y = 0;
    if (global_mouse_x > 315) global_mouse_x = 315;
    if (global_mouse_y > 195) global_mouse_y = 195;
}

// 3. VISUAL CURSOR MANAGEMENT
void show_mouse(int x, int y) {
    // Save background
    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 4; j++) {
            mouse_back_buffer[i * 4 + j] = get_pixel(x + j, y + i);
        }
    }
    // Draw a simple 4x4 white cursor
    draw_rect(x, y, 4, 4, COLOR_WHITE);
}

void hide_mouse(int x, int y) {
    // Restore background
    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 4; j++) {
            put_pixel(x + j, y + i, mouse_back_buffer[i * 4 + j]);
        }
    }
}

// 4. GETTERS
int mouse_get_button() { return last_button_state; }
int mouse_get_x()      { return global_mouse_x; }
int mouse_get_y()      { return global_mouse_y; }
