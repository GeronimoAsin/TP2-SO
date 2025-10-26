#ifndef PRIORITY_QUEUE_H
#define PRIORITY_QUEUE_H

#include "../memoryManager/memoryManager.h"
#include "pcb.h"

typedef struct PriorityQueueCDT * PriorityQueueADT;

PriorityQueueADT createPriorityQueue(MemoryManagerADT memoryManager);

void enqueue(PriorityQueueADT pq, PCB *process);

PCB* dequeue(PriorityQueueADT pq);

PCB* removeFromQueue(PriorityQueueADT pq, PCB *process);  

int isEmpty(PriorityQueueADT pq); 

PCB* peek(PriorityQueueADT pq);  

int size(PriorityQueueADT pq);  

void checkPriorityQueue(PriorityQueueADT pq); 

void destroyPriorityQueue(PriorityQueueADT pq);

#endif // PRIORITY_QUEUE_H