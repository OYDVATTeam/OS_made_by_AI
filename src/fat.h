#ifndef FAT_H
#define FAT_H

#include "libc/stdint.h"

// 1. BIOS Parameter Block (The first sector of the disk)
typedef struct {
    uint8_t  boot_jmp[3];
    char     oem_name[8];
    uint16_t bytes_per_sector;
    uint8_t  sectors_per_cluster;
    uint16_t reserved_sector_count;
    uint8_t  table_count;
    uint16_t root_entry_count;
    uint16_t total_sectors_16;
    uint8_t  media_type;
    uint16_t table_size_16;
    uint16_t sectors_per_track;
    uint16_t head_side_count;
    uint32_t hidden_sector_count;
    uint32_t total_sectors_32;
    
    // Extended FAT16 Fields
    uint8_t  drive_number;
    uint8_t  reserved;
    uint8_t  boot_signature;
    uint32_t volume_id;
    char     volume_label[11];
    char     file_system_type[8];
} __attribute__((packed)) FAT_BS;

// 2. Directory Entry (Represents a file or folder)
typedef struct {
    char     name[8];
    char     ext[3];
    uint8_t  attributes;       // 0x10=Subdir, 0x20=Archive, 0x0F=LongName
    uint8_t  reserved;
    uint8_t  creation_time_ms;
    uint16_t creation_time;
    uint16_t creation_date;
    uint16_t last_access_date;
    uint16_t cluster_high;     // Always 0 in FAT16
    uint16_t last_mod_time;
    uint16_t last_mod_date;
    uint16_t cluster_low;      // The Starting Cluster of the file data
    uint32_t size;             // File size in bytes
} __attribute__((packed)) DirectoryEntry;

// --- Function Prototypes ---

// Reads the Boot Sector and calculates filesystem offsets
void fat_init();

// Draws a list of files to the screen at coordinates (x, y)
void list_files(int x, int y);

// Reads a file into a buffer.
// Returns 1 if found and read, 0 if not found.
// WARNING: buffer must be large enough to hold the file!
int fat_read_file(const char* filename, uint8_t* buffer);

#endif
