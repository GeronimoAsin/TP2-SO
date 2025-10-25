#ifndef PCB_H
#define PCB_H

#include <stdint.h>
#include <stddef.h>

typedef int pid_t;

typedef struct PCB {
    pid_t pid;
    pid_t parentPid;
    size_t priority;
    size_t state; //0 blocked, 1 ready, 2 running, 3 returned
    size_t foreground; //1 foreground, 0 background
    char *name;
    uint64_t *stackPointer;
    uint64_t *basePointer;
    uint64_t *instructionPointer;
    size_t stackSize;
    size_t argc;
    char **argv;
} PCB;

#endif // PCB_H
