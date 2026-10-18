#include <stdint.h>

#define VIDEO_MEMORY (volatile uint16_t*)0xB8000
#define WHITE_ON_BLACK 0x0F

void print_at(const char* str, int col, int row);
void syscall(uint32_t syscall_number, uint32_t arg1, uint32_t arg2, uint32_t arg3);
void kprint(const char* str);