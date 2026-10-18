
#include <stdlib.h>
#include "exe.h"
#include "../../kernel/ram/ram.h"
#include "../../lib/libc/mem.h"
#include "../../kernel/fat16.h"
#include "../../drivers/screen.h"

int exe_load(const char* name, const char* ext) {
    uint8_t sector[512];
    int found = 0;

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

    uint8_t* code = NULL;
   

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

                code = malloc(file_size + sizeof(evx_header_t));
                if (!code) return -2; // erro malloc

                while (cluster < 0xFFF8) {
                    uint32_t first_sector_of_cluster = data_start_sector + (cluster - 2) * sectors_per_cluster;

                    
                    for (int s = 0; s < sectors_per_cluster; s++) {
                        ata_read_sector(first_sector_of_cluster + s, sector);

                        for (int b = 0; b < 512 && bytes_read < file_size; b++) {
                            // putc(sector[b], COLOR_FILE);
                            ata_read_sector(first_sector_of_cluster + s, sector);
                            uint32_t to_copy = (file_size - bytes_read) < 512 ? (file_size - bytes_read) : 512;
                            memcpy(code + bytes_read, sector, to_copy);
                            bytes_read += to_copy;
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
        return 3;
    }
    else {
        // Aqui lê o header do executável que tá no começo do código carregado
        evx_header_t* header = (evx_header_t*) code;

        uint32_t entry_point = header->entry_point;
        uint32_t code_size = header->code_size;
        uint32_t bss_size = header->bss_size;
        uint32_t stack_size = header->stack_size;
        uint32_t heap_size = header->heap_size;

        if (memcmp(header->magic, "EVX\0", 4) != 0) {
            kprint("Invalid file format!\n");
            return 1;
        }

        // Aloca memória total
        uint8_t* mem = malloc(code_size + bss_size + stack_size + heap_size);
        if (!mem) {
            kprint("Erro malloc\n");
            return 2;
         } // erro malloc
         

        /*kprint("Endereco de mem: 0x");
        kprint_hex((uint32_t)mem);
        kprint("\nEntry point offset: 0x");
        kprint_hex(entry_point);
        kprint("\nEndereco real do entry: 0x");
        kprint_hex((uint32_t)(mem + entry_point));
        kprint("\n");
        kprint("Tamanho do cOdigo: 0x");
        kprint_hex(code_size);*/

        

        // Copia código para mem
        memcpy(mem, code + sizeof(evx_header_t), code_size);

        // Zera BSS
        memset(mem + code_size, 0, bss_size);

        free(code);

        
        // Chama entry point (offset dentro do mem)
        void (*entry)() = (void (*)()) (mem + entry_point);

        uint32_t* stack_top = (uint32_t*)(mem + code_size + bss_size + stack_size);
        asm volatile("mov %0, %%esp" :: "r"(stack_top));

        entry();
       

        return 0;
    }
}
