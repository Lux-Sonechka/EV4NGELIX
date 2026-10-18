#ifndef MEM_H
#define MEM_H

#include "../../cpu/types.h"

void memory_copy(u8 *source, u8 *dest, u32 nbytes);
void *memset(void *ptr, int value, unsigned int num);
u32 kmalloc(u32 size, int align, u32 *phys_addr);

#endif
