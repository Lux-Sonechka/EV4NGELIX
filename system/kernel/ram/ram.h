#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

#define MAX_PROCESSES 8
#define PROCESS_RAM_SIZE 0x10000  // 4 KB por processo

#define HEAP_SIZE 0x100000  // 64 KB heap
static uint8_t heap[HEAP_SIZE];

typedef struct block_header {
    size_t size;               // Tamanho do bloco (sem contar o header)
    bool free;                 // Está livre?
    struct block_header* next; // Próximo bloco
} block_header_t;

static block_header_t* free_list = NULL;

extern uint8_t process_memory[MAX_PROCESSES][PROCESS_RAM_SIZE];

void heap_init();
void* malloc(size_t size);
void free(void* ptr);
void meminfo();