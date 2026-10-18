#ifndef FAT16_H
#define FAT16_H

#include <stdint.h>
#pragma once

#pragma pack(push, 1)

#define BYTES_PER_SECTOR 512
#define SECTORS_PER_CLUSTER 1
#define RESERVED_SECTORS 1
#define NUM_FATS 2
#define ROOT_ENTRIES 512
#define SECTORS_PER_FAT 32 // aproximadamente
#define ROOT_DIR_SECTOR (RESERVED_SECTORS + NUM_FATS * SECTORS_PER_FAT)
#define CLUSTER_BEGIN_SECTOR (ROOT_DIR_SECTOR + (ROOT_ENTRIES * 32 / BYTES_PER_SECTOR))
#define FAT_SECTOR 1



#define FAT_OEM_NAME_LENGTH 8
#define FAT_LABEL_LENGTH 11
#define FAT_FS_TYPE_LENGTH 8
#define FAT_OS_BOOT_CODE_LENGTH 448

typedef unsigned int size_t;

typedef struct {
    uint8_t jump[3];
    char oem[8];
    uint16_t bytes_per_sector;
    uint8_t sectors_per_cluster;
    uint16_t reserved_sectors;
    uint8_t num_fats;
    uint16_t root_entries;
    uint16_t root_entry_count;
    uint16_t total_sectors;
    uint8_t media_descriptor;
    uint16_t sectors_per_fat;
    uint16_t sectors_per_track;
    uint16_t heads;
    uint32_t hidden_sectors;
    uint32_t large_sector_count;
} __attribute__((packed)) fat16_bpb_t;

typedef struct {
    char     filename[8];         // Nome do arquivo (sem extensão)
    char     ext[3];              // Extensão (sem ponto)
    uint8_t  attributes;          // Atributos (ex: diretório, sistema, etc)
    uint8_t  reserved;            // Reservado (não usado)
    uint8_t  create_time_tenths;  // Criado em décimos de segundo
    uint16_t create_time;         // Hora de criação
    uint16_t create_date;         // Data de criação
    uint16_t access_date;         // Último acesso
    uint16_t ignored_fat16;       // *** Ignorado em FAT16 (usado só no FAT32) ***
    uint16_t modify_time;         // Última modificação - hora
    uint16_t modify_date;         // Última modificação - data
    uint16_t starting_cluster;    // Cluster inicial (FAT16 usa esse aqui!)
    uint32_t file_size;           // Tamanho do arquivo em bytes
} __attribute__((packed)) fat16_dir_entry_t;



void list_files();
void ata_read_sector(uint32_t lba, uint8_t* buffer);
void fat16_read_file(const char* filename, uint8_t* buffer);
void putc(char c, int attribute_byte);
void puts(const char* str);
void fat16_init();
void fat16_read_directory(uint8_t* buffer);
void fat16_read_cluster(uint32_t cluster, uint8_t* buffer);
void fat16_read_file(const char* filename, uint8_t* buffer);
void fat16_read_directory(uint8_t* buffer);
void fat16_read_cluster(uint32_t cluster, uint8_t* buffer);
void fat16_read_file(const char* filename, uint8_t* buffer);
void fat16_read_directory(uint8_t* buffer);
void read_file(const char* name, const char* ext);
void test_info();


#endif // FAT16_H