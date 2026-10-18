#include <stdbool.h>
#include "../kernel/kernel.h"
#include "../kernel/fat16.h"
#include "../drivers/screen.h"
#include "../cpu/isr.h"
#include "../lib/libc/string.h"
#include "../lib/libc/mem.h"
#include "../kernel/low_level.h"

int offset;

volatile uint16_t* vga_buffer = (uint16_t*)0xB8000;
uint8_t cursor_row = -1;
uint8_t cursor_col = -1;
#define NULL ((void*)0)



void *memcpy(void *dest, const void *src, uint32_t count) {
    uint8_t *d = (uint8_t*)dest;
    const uint8_t *s = (const uint8_t*)src;

    for (uint32_t i = 0; i < count; i++)
        d[i] = s[i];

    return dest;
}
int memcmp(const void *ptr1, const void *ptr2, uint32_t count) {
    const uint8_t *a = (const uint8_t*)ptr1;
    const uint8_t *b = (const uint8_t*)ptr2;

    for (uint32_t i = 0; i < count; i++) {
        if (a[i] != b[i])
            return (int)(a[i] - b[i]);
    }
    return 0;
}


void putc(char c, int attribute_byte) {

    unsigned char *vidmem = (unsigned char*) VIDEO_ADDRESS;
    offset = get_cursor();
    cursor_row = get_offset_row(offset);
    cursor_col = get_offset_col(offset);

    if (c == '\n') {
        cursor_row++;
        cursor_col = 0;
        return;
    }
    if (cursor_col >= 0 && cursor_col >= 0)
		offset = get_screen_offset(cursor_col, cursor_row);
	else
		offset = get_cursor();

	if (c == '\n') {
		int rows = offset / (2 * MAX_COLS);
		offset = get_screen_offset(MAX_COLS, rows);
	} else {
		vidmem[offset] = c;
		vidmem[offset+1] = attribute_byte;
		offset += 2;
	}

	
	set_cursor(offset);

    uint16_t position = cursor_row * 80 + cursor_col;
    vga_buffer[position] = (attribute_byte << 8) | c; // cor cinza claro no fundo preto

    cursor_col++;
    if (cursor_col >= 80) {
        cursor_col = 0;
        cursor_row++;
    }
}


static inline void outb(uint16_t port, uint8_t val) {
    asm volatile ("outb %0, %1" : : "a"(val), "Nd"(port));
}

static inline uint8_t inb(uint16_t port) {
    uint8_t ret;
    asm volatile ("inb %1, %0" : "=a"(ret) : "Nd"(port));
    return ret;
}

static inline uint16_t inw(uint16_t port) {
    uint16_t ret;
    asm volatile ("inw %1, %0" : "=a"(ret) : "Nd"(port));
    return ret;
}

static inline void io_wait() {
    asm volatile ("nop");
    asm volatile ("nop");
    asm volatile ("nop");
    asm volatile ("nop");
    asm volatile ("nop");
    asm volatile ("nop");
    asm volatile ("nop");
    asm volatile ("nop");
}

void ata_read_sector(uint32_t lba, uint8_t* buffer) {
    outb(0x1F6, 0xE0 | ((lba >> 24) & 0x0F));
    io_wait();
    outb(0x1F2, 1);
    io_wait();
    outb(0x1F3, (uint8_t)(lba & 0xFF));
    io_wait();
    outb(0x1F4, (uint8_t)((lba >> 8) & 0xFF));
    io_wait();
    outb(0x1F5, (uint8_t)((lba >> 16) & 0xFF));
    io_wait();
    outb(0x1F7, 0x20);

    while ((inb(0x1F7) & 0x88) != 0x08) ;
    io_wait(); // Atraso extra antes de ler os dados

    if (inb(0x1F7) & 0x01) {
        kprint("ATA Error!\n");
        return;
    }

    for (int i = 0; i < 256; i++) {
        uint16_t data = inw(0x1F0);
        buffer[i * 2] = data & 0xFF;
        buffer[i * 2 + 1] = (data >> 8) & 0xFF;
        io_wait(); // Atraso extra após ler cada palavra
    }
}


void list_files() {
    uint8_t sector[512];
    ata_read_sector(0, sector); // Lê MBR ou VBR

    fat16_bpb_t* bpb = (fat16_bpb_t*) (sector);

    uint32_t root_dir_sectors = ((bpb->root_entries * 32) + (bpb->bytes_per_sector - 1)) / bpb->bytes_per_sector;
    uint32_t fat_size = bpb->sectors_per_fat;
    uint32_t first_root_dir_sector = bpb->reserved_sectors + (bpb->num_fats * fat_size);

    // Lendo setor de root
    for (uint32_t i = 0; i < root_dir_sectors; i++) {
        ata_read_sector(first_root_dir_sector + i, sector);

        fat16_dir_entry_t* entry = (fat16_dir_entry_t*) sector;

        for (int j = 0; j < 512 / sizeof(fat16_dir_entry_t); j++) {
            if (entry[j].filename[0] == 0x00) {
                // Fim da lista
                return;
            }
            if ((entry[j].attributes & 0x0F) == 0) { // Se não for volume label
                set_vga_color(COLOR_FILE);
                kprint("-- ");
                for (int k = 0; k < 8; k++) if (entry[j].filename[k] != ' ') {
                    
                    putc(entry[j].filename[k], COLOR_FILE);
                }
                putc('.', COLOR_FILE); 
                for (int k = 0; k < 3; k++) if (entry[j].ext[k] != ' ') putc(entry[j].ext[k], COLOR_FILE);
                kprint("\n");
                set_vga_color(COLOR_DEFAULT);
            }
        }
    }
}

void read_file(const char* name, const char* ext) {
    uint8_t sector[512];
    int found = 0;
    int color = VGA_LIGHT_GRAY;

    // Passo 1: Lê setor 0 e interpreta BPB
    ata_read_sector(0, sector);
    fat16_bpb_t* bpb = (fat16_bpb_t*) sector;

    uint16_t root_entries = bpb->root_entries;
    uint16_t bytes_per_sector = bpb->bytes_per_sector;
    uint16_t sectors_per_fat = bpb->sectors_per_fat;
    uint16_t reserved_sectors = bpb->reserved_sectors;
    uint16_t num_fats = bpb->num_fats;
    uint16_t sectors_per_cluster = bpb->sectors_per_cluster;

    uint16_t root_dir_sectors = ((root_entries * 32) + (bytes_per_sector - 1)) / bytes_per_sector;
    uint32_t first_root_dir_sector = reserved_sectors + (num_fats * sectors_per_fat);
    uint32_t data_start_sector = first_root_dir_sector + root_dir_sectors;

   

    // Passo 2: Procura o arquivo na root
    for (uint32_t i = 0; i < root_dir_sectors; i++) {
        ata_read_sector(first_root_dir_sector + i, sector);
        fat16_dir_entry_t* entry = (fat16_dir_entry_t*) sector;

        for (int j = 0; j < 512 / sizeof(fat16_dir_entry_t); j++) {
            if (entry[j].filename[0] == 0x00) {break;}
            if (entry[j].filename[0] == 0xE5) continue;
            if ((entry[j].attributes & 0x0F) != 0x00) continue;


            // Comparação de nome
            if (memcmp(entry[j].filename, name, 8) == 0 &&
                memcmp(entry[j].ext, ext, 3) == 0) {


                uint16_t cluster = entry[j].starting_cluster;
                uint32_t file_size = entry[j].file_size;
                uint32_t bytes_read = 0;
                found = 1;

                while (cluster < 0xFFF8) {
                    uint32_t first_sector_of_cluster = data_start_sector + (cluster - 2) * sectors_per_cluster;

                    
                    for (int s = 0; s < sectors_per_cluster; s++) {
                        ata_read_sector(first_sector_of_cluster + s, sector);

                        for (int b = 0; b < 512 && bytes_read < file_size; b++) {
                            if (sector[b] == 0x0A) {
                                kprint("\n");
                            }
                            else if (sector[b] == 0x26 && sector[b+1] == 0x52) { //"&R") {
                                color = VGA_RED;
                                b++;
                            }
                            else if (sector[b] == 0x26 && sector[b+1] == 0x47) { //"&G") {
                                color = VGA_GREEN;
                                b++;
                            }
                            else if (sector[b] == 0x26 && sector[b+1] == 0x42) { //"&B") {
                                color = VGA_BLUE;
                                b++;
                            }
                            else if (sector[b] == 0x26 && sector[b+1] == 0x59) { //"&Y") {
                                color = VGA_YELLOW;
                                b++;
                            }
                            else if (sector[b] == 0x26 && sector[b+1] == 0x4D) { //"&M") {
                                color = VGA_MAGENTA;
                                b++;
                            }
                            else if (sector[b] == 0x26 && sector[b+1] == 0x43) { //"&C") {
                                color = VGA_CYAN;
                                b++;
                            }
                            else if (sector[b] == 0x26 && sector[b+1] == 0x57) { //"&W") {
                                color = VGA_WHITE;
                                b++;
                            }
                            else {
                                putc(sector[b], color);
                            }

                            bytes_read++;
                        }
                    }

                    // Leitura da FAT para próximo cluster
                    uint32_t fat_sector = reserved_sectors + (cluster * 2) / 512;
                    ata_read_sector(fat_sector, sector);
                    cluster = *((uint16_t*)&sector[(cluster * 2) % 512]);
                }

            }
        }
    }
    if (!found) {
        kprint("Error: File not found!\n");
        set_vga_color(COLOR_INFO);
        kprint("Try again or run 'root' for file list from root directory\n or 'dir' for current directory.\n ");
    }
    
}
