#ifndef PRIORITY_QUEUE_H
#define PRIORITY_QUEUE_H

#include <stdint.h>
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

typedef struct PriorityQueueCDT * PriorityQueueADT;

PriorityQueueADT createPriorityQueue();

void enqueue(PriorityQueueADT pq, PCB *process);

PCB* dequeue(PriorityQueueADT pq);

void checkPriorityQueue(PriorityQueueADT pq); //Para usar dsps de cambiar prioridad

void destroyPriorityQueue(PriorityQueueADT pq);

#endif // PRIORITY_QUEUE_H