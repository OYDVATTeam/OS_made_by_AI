#include "io.h"

#define ATA_DATA         0x1F0
#define ATA_SECCOUNT     0x1F2
#define ATA_LBA_LOW      0x1F3
#define ATA_LBA_MID      0x1F4
#define ATA_LBA_HIGH     0x1F5
#define ATA_DRIVE_SEL    0x1F6
#define ATA_COMMAND      0x1F7
#define ATA_STATUS       0x1F7

void ata_read_sector(int lba, unsigned char* buffer) {
    // Select drive (LBA mode)
    outb(ATA_DRIVE_SEL, 0xE0 | ((lba >> 24) & 0x0F));
    outb(ATA_SECCOUNT, 1);
    outb(ATA_LBA_LOW,  (unsigned char)lba);
    outb(ATA_LBA_MID,  (unsigned char)(lba >> 8));
    outb(ATA_LBA_HIGH, (unsigned char)(lba >> 16));
    outb(ATA_COMMAND,  0x20); // Read Sectors

    // Wait for drive (BSY bit 7 to clear, DRQ bit 3 to set)
    while (inb(ATA_STATUS) & 0x80);
    while (!(inb(ATA_STATUS) & 0x08));

    // Read 256 words (512 bytes)
    unsigned short* ptr = (unsigned short*)buffer;
    for (int i = 0; i < 256; i++) {
        ptr[i] = inw(ATA_DATA);
    }
}
