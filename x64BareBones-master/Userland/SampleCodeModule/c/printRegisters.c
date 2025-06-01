#include <stdint.h>
#include "userlib.h"
extern void syscall(uint64_t rax, uint64_t rbx, uint64_t rdx, uint64_t rcx);

void print_registers() {
    uint64_t* regs = syscalls(4,0,0,0);

    const char* reg_names[16] = {
        "RAX", "RBX", "RCX", "RDX",
        "RSI", "RDI", "RSP", "RBP",
        "R8",  "R9",  "R10", "R11",
        "R12", "R13", "R14", "R15"
    };

    for (int i = 0; i < 16; i++) {
        printf("%s = %s\n", reg_names[i], numToHex(regs[i]));
    }
}