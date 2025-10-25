#ifndef PRIORITY_QUEUE_H
#define PRIORITY_QUEUE_H

#include "pcb.h"

typedef struct PriorityQueueCDT * PriorityQueueADT;

PriorityQueueADT createPriorityQueue();

void enqueue(PriorityQueueADT pq, PCB *process);

PCB* dequeue(PriorityQueueADT pq);

void checkPriorityQueue(PriorityQueueADT pq); //Para usar dsps de cambiar prioridad

void destroyPriorityQueue(PriorityQueueADT pq);

#endif // PRIORITY_QUEUE_H