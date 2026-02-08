// kernel.c
#define VGA_WIDTH       80
#define VGA_HEIGHT      25
#define VGA_BUFFER      ((volatile unsigned short *)0xB8000)

#define COLOR_WHITE_ON_BLACK    0x0F

// Forward declarations
static void delay(void);
static void clear_line(void);
static void print_at(const char *str);

// ───────────────────────────────────────────────
// Very rough busy-wait delay (~0.1–0.4 s depending on CPU)
// ───────────────────────────────────────────────
static void delay(void) {
    volatile unsigned int i;
    for (i = 0; i < 0xB8FA0; i++) {
        asm volatile("" : : : "memory");   // prevent optimization
    }
}

// Clear ~16 characters worth of space (our animation area)
static void clear_line(void) {
    volatile unsigned short *pos = VGA_BUFFER;
    for (int i = 0; i < 32; i++) {         // 16 chars × 2 bytes
        pos[i] = 0x0F20;                   // space + white-on-black
    }
}

static void print_at(const char *str) {
    volatile unsigned short *pos = VGA_BUFFER;
    while (*str) {
        *pos++ = (*str++) | (COLOR_WHITE_ON_BLACK << 8);
    }
}

// ───────────────────────────────────────────────
// Main kernel function – called from assembly
// ───────────────────────────────────────────────
void kernel_main(void) {
    const char *frames[] = {
        "Hello >",
        "Hello -",
        "Hello <",
        "Hello |"
    };

    while (1) {
        for (int i = 0; i < 4; i++) {
            clear_line();
            print_at(frames[i]);
            delay();
        }
    }
}
