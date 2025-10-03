#ifndef PRIORITY_QUEUE_H
#define PRIORITY_QUEUE_H

#include <stdint.h>
typedef int pid_t;

typedef struct PriorityQueueCDT * PriorityQueueADT;

PriorityQueueADT createPriorityQueue();

void enqueue(PriorityQueueADT pq, PCB *process);

PCB* dequeue(PriorityQueueADT pq);

#endif // PRIORITY_QUEUE_H