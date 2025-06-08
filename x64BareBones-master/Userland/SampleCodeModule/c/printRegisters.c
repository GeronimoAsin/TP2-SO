#include <stdint.h>
#include "userlib.h"

extern void syscall(uint64_t rax, uint64_t rbx, uint64_t rdx, uint64_t rcx);

void print_registers() {
    uint64_t regs[16];
    syscall(4, (uint64_t) &regs, 0, 0);  // Llamada a syscall con el número 4 (getRegisters)

    const char* reg_names[16] = {
        "RAX", "RBX", "RCX", "RDX",
        "RSI", "RDI", "RSP", "RBP",
        "R8",  "R9",  "R10", "R11",
        "R12", "R13", "R14", "R15"
    };

//test printf funcionando ok 
/*
 uint64_t test_values[] = {
        0x0,
        0x1,
        0xDEADBEEF,
        0x123456789ABCDEF0,
        0xFFFFFFFFFFFFFFFF,
        0x00000000FFFFFFFF,
        0xABCDEF1234567890
    };
    int n = sizeof(test_values) / sizeof(test_values[0]);
    for (int i = 0; i < n; i++) {
        printf("Test %d: 0x%llx\n", i, test_values[i]);
    }

*/

    //print de los registros en formato hexadecimal
    for (int i = 0; i < 16; i++) {
        printf("%s = 0x %llx\n", reg_names[i], (unsigned long long)regs[i]);
    }
}