#include "notepad.h"
#include "fat.h"
#include "vga.h"
#include "libc/string.h"
#include "libc/stdio.h"

#define MAX_FILE_SIZE 8096
static char file_content[MAX_FILE_SIZE];
static char current_filename[13];
static int is_open = 0;

void notepad_open(const char* filename) {
    // Clear old buffer
    memset(file_content, 0, MAX_FILE_SIZE);
    
    // Attempt to read from FAT
    if (fat_read_file(filename, (uint8_t*)file_content)) {
        strcpy(current_filename, filename);
        is_open = 1;
    } else {
        // Optional: show error message
        is_open = 0;
    }
}

void notepad_draw() {
    if (!is_open) return;

    // Window dimensions
    int win_x = 50, win_y = 50;
    int win_w = 220, win_h = 150;

    // Draw Window Border/Background
    // (Assuming you have a draw_rect or similar, otherwise use draw_pixel)
    fill_rect(win_x, win_y, win_w, win_h, COLOR_LIGHT_GRAY); 
    fill_rect(win_x, win_y, win_w, 15, COLOR_BLUE); // Title bar

    // Draw Title
    char title[32];
    sprintf(title, "Notepad - %s", current_filename);
    draw_string(win_x + 5, win_y + 2, title, COLOR_WHITE);

    // Draw Content (limit characters to fit window)
    // We can use a basic wrap or just draw lines
    draw_string(win_x + 5, win_y + 20, file_content, COLOR_BLACK);
}

void notepad_close() {
    is_open = 0;
}
