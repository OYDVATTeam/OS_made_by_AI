#include "fat.h"
#include "vga.h"
#include "font.h"
#include "libc/string.h"
#include "libc/stdio.h"
#include "libc/ctype.h"

// External hardware driver
extern void ata_read_sector(uint32_t lba, uint8_t* buffer);

// Global Filesystem State
static FAT_BS boot_sector;
static uint32_t root_start_sector;
static uint32_t data_start_sector;
static uint8_t disk_buffer[512]; // General purpose buffer
static uint8_t fat_table_buffer[512]; // Buffer specifically for FAT lookups

// --- Helper: Convert Cluster Number to Disk Sector (LBA) ---
static uint32_t cluster_to_lba(uint16_t cluster) {
    return data_start_sector + ((cluster - 2) * boot_sector.sectors_per_cluster);
}

// --- Helper: Standardize Filename ---
// Converts "test.txt" -> "TEST    TXT" for comparison
static void to_dos_filename(const char* input, char* output) {
    memset(output, ' ', 11);
    
    int i = 0, j = 0;
    
    // Copy Name
    while (input[i] && input[i] != '.' && j < 8) {
        output[j++] = toupper(input[i++]);
    }
    
    // Skip to dot if present
    if (input[i] == '.') i++;
    
    // Copy Extension
    j = 8;
    while (input[i] && j < 11) {
        output[j++] = toupper(input[i++]);
    }
}

// --- 1. Initialize Filesystem ---
void fat_init() {
    // Read the Boot Parameter Block (Sector 0)
    ata_read_sector(0, (uint8_t*)&boot_sector);

    // Verify Signature (0x55AA)
    // Note: boot_sector.boot_signature is at offset 510 in the sector, 
    // but our struct might vary slightly depending on packing. 
    // Usually, we trust it if we are booting.

    // Calculate Start of Root Directory
    // Root = Reserved + (FATs * FAT_Size)
    root_start_sector = boot_sector.reserved_sector_count + 
                        (boot_sector.table_count * boot_sector.table_size_16);

    // Calculate Start of Data Region
    // Data = Root_Start + (Root_Entries * 32 bytes / 512 bytes per sector)
    uint32_t root_dir_sectors = (boot_sector.root_entry_count * 32) / 512;
    data_start_sector = root_start_sector + root_dir_sectors;
}

// --- 2. List Files (Visual) ---
void list_files(int x, int y) {
    ata_read_sector(root_start_sector, disk_buffer);
    DirectoryEntry* entries = (DirectoryEntry*)disk_buffer;

    char label_buf[32];
    draw_string(x, y, "FILES:", COLOR_YELLOW);
    y += 12;

    for (int i = 0; i < 16; i++) {
        if (entries[i].name[0] == 0x00) break; // End of dir
        if (entries[i].name[0] == 0xE5) continue; // Deleted
        if (entries[i].attributes & 0x0F) continue; // Long filename LFN (skip)

        // Format name "FILENAME.EXT"
        char name[13];
        int k = 0;
        for (int j = 0; j < 8; j++) {
            if (entries[i].name[j] != ' ') name[k++] = entries[i].name[j];
        }
        name[k++] = '.';
        for (int j = 0; j < 3; j++) {
            if (entries[i].ext[j] != ' ') name[k++] = entries[i].ext[j];
        }
        name[k] = '\0';

        // Print Name and Size
        draw_string(x, y, name, COLOR_WHITE);
        sprintf(label_buf, "%d b", entries[i].size);
        draw_string(x + 100, y, label_buf, COLOR_GRAY);
        
        y += 10;
    }
}

// --- 3. Read File Content ---
// returns 1 on success, 0 on failure
int fat_read_file(const char* filename, uint8_t* buffer) {
    char dos_name[11];
    to_dos_filename(filename, dos_name);

    // 1. Search Root Directory
    ata_read_sector(root_start_sector, disk_buffer);
    DirectoryEntry* entries = (DirectoryEntry*)disk_buffer;
    
    int file_found = -1;
    for (int i = 0; i < 16; i++) {
        if (entries[i].name[0] == 0x00) break;
        // Compare 11 bytes (Name + Ext)
        if (memcmp(entries[i].name, dos_name, 11) == 0) {
            file_found = i;
            break;
        }
    }

    if (file_found == -1) return 0; // Not found

    // 2. Follow Cluster Chain
    uint16_t current_cluster = entries[file_found].cluster_low;
    uint32_t file_size = entries[file_found].size;
    uint32_t bytes_read = 0;
    uint8_t* buf_ptr = buffer;

    while (bytes_read < file_size) {
        // A. Read Data Cluster
        uint32_t sector = cluster_to_lba(current_cluster);
        ata_read_sector(sector, buf_ptr);
        
        buf_ptr += 512;
        bytes_read += 512;

        // B. Look up Next Cluster in FAT Table
        // FAT Offset = Cluster * 2 (since entries are 16-bit)
        uint32_t fat_offset = current_cluster * 2;
        uint32_t fat_sector = boot_sector.reserved_sector_count + (fat_offset / 512);
        uint32_t fat_index = fat_offset % 512;

        // Read the FAT sector
        ata_read_sector(fat_sector, fat_table_buffer);
        
        // Get next cluster from the table
        // We cast to uint16_t* pointer to read 2 bytes
        uint16_t next_cluster = *(uint16_t*)&fat_table_buffer[fat_index];

        if (next_cluster >= 0xFFF8) break; // End of file marker (EOF)
        current_cluster = next_cluster;
    }
    
    // Null terminate the buffer so it can be printed as a string
    buffer[file_size] = '\0';
    return 1;
}
