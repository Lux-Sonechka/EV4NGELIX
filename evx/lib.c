// screen.c - versão mínima sem dependências

#include <stdint.h>


void syscall(uint32_t syscall_number, uint32_t arg1, uint32_t arg2, uint32_t arg3) {
    __asm__ __volatile__(
        "int $0x80"
        : // Sem saída
        : "a"(syscall_number), "b"(arg1), "c"(arg2), "d"(arg3) // Argumentos nos registradores
    );
}

void print_at(const char* str, int col, int row) {
    syscall(2, (uint32_t)str, col, row);
}
void kprint(const char* str) {
    syscall(1, (uint32_t)str, 0, 0);
}