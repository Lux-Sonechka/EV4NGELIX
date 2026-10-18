#include <stdint.h>
#include <stddef.h>
#include "ram.h"
#include "../../drivers/screen.h"
#include "../../lib/libc/string.h"
#include "../../lib/libc/mem.h"
#include "../../kernel/low_level.h"

void heap_init() {
    free_list = (block_header_t*)heap;
    free_list->size = HEAP_SIZE - sizeof(block_header_t);
    free_list->free = 1;
    free_list->next = NULL;
}

// Aloca memória do heap (simples first-fit)
void* malloc(size_t size) {
    block_header_t* current = free_list;
    block_header_t* prev = NULL;

    while (current) {
        if (current->free && current->size >= size) {
            // Se couber, fragmenta o bloco (se sobrar espaço)
            if (current->size > size + sizeof(block_header_t)) {
                block_header_t* new_block = (block_header_t*)((uint8_t*)current + sizeof(block_header_t) + size);
                new_block->size = current->size - size - sizeof(block_header_t);
                new_block->free = 1;
                new_block->next = current->next;

                current->size = size;
                current->next = new_block;
            }

            current->free = 0;
            return (void*)((uint8_t*)current + sizeof(block_header_t));
        }
        prev = current;
        current = current->next;
    }

    // Sem espaço
    kprint("Heap overflow!\n");
    return NULL;
}

void free(void* ptr) {
    if (!ptr) return;

    block_header_t* block = (block_header_t*)((uint8_t*)ptr - sizeof(block_header_t));
    block->free = 1;

    // Junta blocos livres adjacentes (coalescing)
    block_header_t* current = free_list;
    while (current) {
        if (current->free && current->next && current->next->free) {
            current->size += sizeof(block_header_t) + current->next->size;
            current->next = current->next->next;
        } else {
            current = current->next;
        }
    }
}
void meminfo() {


    size_t total = 0;
    size_t used = 0;
    size_t free = 0;
    int occupied_blocks = 0;
    int free_blocks = 0;

    block_header_t* current = free_list;
    while (current) {
        total += current->size;
        if (current->free) {
            free += current->size;
            free_blocks++;
        } else {
            used += current->size;
            occupied_blocks++;
        }
        current = current->next;
    }
    set_vga_color(VGA_LIGHT_GRAY);
    kprint("Total heap: ");
    char total_c[10];
    int_to_ascii(total, total_c);
    kprint(total_c);
    kprint(" bytes\n");

    char used_c[10];
    int_to_ascii(used, used_c);
    kprint("Used heap: ");
    kprint(used_c);
    kprint(" bytes\n");

    char free_c[10];
    int_to_ascii(free, free_c);
    kprint("Free heap: ");
    kprint(free_c);
    kprint(" bytes\n");

    char occupied_blocks_c[10];
    int_to_ascii(occupied_blocks, occupied_blocks_c);
    kprint("Occupied blocks: ");
    kprint(occupied_blocks_c);
    kprint(" blocks\n");

    char free_blocks_c[10];
    int_to_ascii(free_blocks, free_blocks_c);
    kprint("Free blocks: ");
    kprint(free_blocks_c);
    kprint(" blocks\n");

    //printf("Usado: %zu bytes\n", used);
    //printf("Livre: %zu bytes\n", free);
    //printf("Blocos ocupados: %d\n", occupied_blocks);
    //printf("Blocos livres: %d\n", free_blocks);
    set_vga_color(VGA_COLOR);
}