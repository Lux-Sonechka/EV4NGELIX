#include <stdint.h>
#include "../../cpu/isr.h"
#include "../../lib/libc/string.h"
#include "../../lib/libc/mem.h"
#include "../../kernel/low_level.h"
#include "../../kernel/fat16.h"

#define NULL ((void*)0)

void user_input (char *input);
void prompt();

extern const char* sigil[];