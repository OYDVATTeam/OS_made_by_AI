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
