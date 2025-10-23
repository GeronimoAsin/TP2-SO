#ifndef LIST_H
#define LIST_H

#include <stdint.h>
typedef struct ListCDT * ListADT;
typedef int pid_t;
typedef unsigned int size_t;

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

ListADT createList();

void addToList(ListADT list, PCB *process);

PCB* removeFromList(ListADT list, pid_t processId);

void destroyList(ListADT list);

#endif // LIST_H