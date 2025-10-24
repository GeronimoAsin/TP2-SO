#include <stdint.h>
#include "userlib.h"

extern void syscall(uint64_t rax, uint64_t rbx, uint64_t rdx, uint64_t rcx);
extern void getRegisters(uint64_t *regs);

void print_registers() {
    uint64_t regs[18] = {0};
    getRegisters(regs);

    const char* reg_names[18] = {
        "RAX", "RBX", "RCX", "RDX",
        "RSI", "RDI", "RSP", "RBP",
        "R8",  "R9",  "R10", "R11",
        "R12", "R13", "R14", "R15",  "RIP", "RFLAGS"
    };



    //print de los registros en formato hexadecimal
    for (int i = 0; i < 18; i++) {
        printf("%s = 0x%llx\n", reg_names[i], (unsigned long long)regs[i]);
    }
    return;


}