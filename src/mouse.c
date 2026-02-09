#include "mouse.h"
#include "io.h"
#include "vga.h"

#define PS2_DATA    0x60
#define PS2_STATUS  0x64
#define PS2_COMMAND 0x64

static int mouse_x = 160;
static int mouse_y = 100;

static unsigned char packet[3];
static int packet_cycle = 0;

/* Wait helpers */
static void wait_input(void) {
    while (inb(PS2_STATUS) & 0x02);
}

static void wait_output(void) {
    while (!(inb(PS2_STATUS) & 0x01));
}

/* Send command to mouse */
static void mouse_write(unsigned char value) {
    wait_input();
    outb(PS2_COMMAND, 0xD4);
    wait_input();
    outb(PS2_DATA, value);
}

/* Read mouse data */
static unsigned char mouse_read(void) {
    wait_output();
    return inb(PS2_DATA);
}

// A small 4x4 buffer to store pixels hidden by the cursor
static unsigned char mouse_back_buffer[16];

void show_mouse(int x, int y) {
    // 1. Save what is currently on the screen at this spot
    for (int i = 0; i < 2; i++) {
        for (int j = 0; j < 2; j++) {
            mouse_back_buffer[i * 2 + j] = get_pixel(x + j, y + i);
        }
    }
    // 2. Draw the actual cursor (defined in gui.c or locally)
    draw_mouse_cursor(x, y);
}

void hide_mouse(int x, int y) {
    // Restore the saved pixels back to the screen
    for (int i = 0; i < 2; i++) {
        for (int j = 0; j < 2; j++) {
            put_pixel(x + j, y + i, mouse_back_buffer[i * 2 + j]);
        }
    }
}

int mouse_get_button() {
    /* REAL HARDWARE CHECK:
       We check Port 0x64 (Status Register). 
       If Bit 0 is 1, the mouse has sent a hardware packet.
    */
    while (inb(0x64) & 0x01) {
        // A PS/2 packet is 3 bytes. We must read all 3 or the mouse will lock up.
        unsigned char status = inb(0x60); // Byte 0: Buttons
        char rel_x = (char)inb(0x60);    // Byte 1: X movement
        char rel_y = (char)inb(0x60);    // Byte 2: Y movement

        // Update the button state from the hardware bit (Bit 0 = Left Click)
        last_button_state = (status & 0x01);

        // Update coordinates so movement still works while checking buttons
        global_mouse_x += rel_x;
        global_mouse_y -= rel_y; 
        
        // Screen clamping
        if (global_mouse_x < 0) global_mouse_x = 0;
        if (global_mouse_y < 0) global_mouse_y = 0;
        if (global_mouse_x > 319) global_mouse_x = 319;
        if (global_mouse_y > 199) global_mouse_y = 199;
    }

    /* Return the actual state. 
       If no new packet arrived, it returns the LAST state the hardware sent.
       This prevents the "0 is not clicked" bug that closes your menu.
    */
    return last_button_state;
}

void mouse_init(void) {
    /* Enable auxiliary device */
    wait_input();
    outb(PS2_COMMAND, 0xA8);

    /* Enable interrupts */
    wait_input();
    outb(PS2_COMMAND, 0x20);
    wait_output();
    unsigned char status = inb(PS2_DATA);
    status |= 0x02;
    wait_input();
    outb(PS2_COMMAND, 0x60);
    wait_input();
    outb(PS2_DATA, status);

    /* Default settings */
    mouse_write(0xF6);
    mouse_read();

    /* Enable packet streaming */
    mouse_write(0xF4);
    mouse_read();
}

void mouse_poll(void) {
    if (!(inb(PS2_STATUS) & 0x01)) return;

    unsigned char data = inb(PS2_DATA);

    /* First byte must have bit 3 set */
    if (packet_cycle == 0 && !(data & 0x08)) return;

    packet[packet_cycle++] = data;

    if (packet_cycle == 3) {
        packet_cycle = 0;

        int dx = (char)packet[1];
        int dy = (char)packet[2];

        mouse_x += dx;
        mouse_y -= dy; // inverted Y

        if (mouse_x < 0) mouse_x = 0;
        if (mouse_y < 0) mouse_y = 0;
        if (mouse_x >= SCREEN_WIDTH)  mouse_x = SCREEN_WIDTH - 1;
        if (mouse_y >= SCREEN_HEIGHT) mouse_y = SCREEN_HEIGHT - 1;
    }
}

int mouse_get_x(void) { return mouse_x; }
int mouse_get_y(void) { return mouse_y; }
