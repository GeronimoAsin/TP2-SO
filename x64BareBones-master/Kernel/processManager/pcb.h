#ifndef PCB_H
#define PCB_H

#include <stdint.h>
#include <stddef.h>

typedef int pid_t;

typedef struct Context {
    uint64_t rsp;
    uint64_t rbp;
    uint64_t rip;
    uint64_t rax;
    uint64_t rbx;
    uint64_t rcx;
    uint64_t rdx;
    uint64_t rsi;
    uint64_t rdi;
    uint64_t r8;
    uint64_t r9;
    uint64_t r10;
    uint64_t r11;
    uint64_t r12;
    uint64_t r13;
    uint64_t r14;
    uint64_t r15;
    uint64_t rflags;
    uint64_t cs;
    uint64_t ss;
    uint64_t align;
} Context;

typedef struct PCB {
    Context context;
    pid_t pid;
    pid_t parentPid;
    size_t priority;
    size_t state; //0 blocked, 1 ready, 2 running, 3 returned
    size_t foreground; //1 foreground, 0 background
    char *name;
    uint64_t *stackBase;
    uint64_t *stackPointer;
    uint64_t *basePointer;
    uint64_t *instructionPointer;
    size_t stackSize;
    size_t argc;
    char **argv;
    size_t waitingToRead;
} PCB;

#endif // PCB_H
