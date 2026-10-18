#include <stdint.h>
#include "../../lib/libc/string.h"
#include "../../kernel/low_level.h"


typedef struct {
    uint8_t  magic[4];      // "EVX\0"
    uint32_t code_size;     // tamanho do código
    uint32_t entry_point;   // offset de onde começa
    uint32_t bss_size;      // opcional: memória não inicializada
    uint32_t version;     // versão do executável
    uint32_t flags;         // flags de execução
    uint32_t stack_size;    // tamanho da pilha
    uint32_t heap_size;     // tamanho do heap
    uint8_t code[];
    // ...pode ter + coisas tipo dependências, syscall version, etc
    // uint8_t code[]; // vem depois do header
} evx_header_t;

int exe_load(const char* name, const char* ext);