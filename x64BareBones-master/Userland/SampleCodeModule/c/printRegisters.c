#include <stdint.h>
#include "userlib.h"

void print_registers() {
    uint64_t regs[16];
    syscall(4, (uint64_t)regs, 0, 0);

    const char* reg_names[16] = {
        "RAX", "RBX", "RCX", "RDX",
        "RSI", "RDI", "RSP", "RBP",
        "R8",  "R9",  "R10", "R11",
        "R12", "R13", "R14", "R15"
    };

    for (int i = 0; i < 16; i++) {
    printf("%s = %d\n", reg_names[i], regs[i]);
        // o para hexadecimal:
    //printf("%s = 0x %llx \n", reg_names[i], (unsigned long long)regs[i]);

    }
}