#include "priorityQueue.h"
#include "../memoryManager/memoryManager.h"
#define NULL ((void*)0)

typedef struct Node {
    PCB *process;
    struct Node *next;
} Node;

typedef struct PriorityQueueCDT {
    Node *first;
    MemoryManagerADT memoryManager;
} PriorityQueueCDT;

PriorityQueueADT createPriorityQueue(MemoryManagerADT memoryManager) {
    PriorityQueueADT pq = (PriorityQueueADT) allocateMemory(memoryManager, sizeof(PriorityQueueCDT));
    pq->first = NULL;
    pq->memoryManager = memoryManager;
    return pq;
}

void enqueue(PriorityQueueADT pq, PCB *process) {
    Node *newNode = (Node *) allocateMemory(pq->memoryManager, sizeof(Node));
    newNode->process = process;
    newNode->next = NULL;

    if (pq->first == NULL || process->priority > pq->first->process->priority) {
        newNode->next = pq->first;
        pq->first = newNode;
    } else {
        Node *current = pq->first;
        while (current->next != NULL && current->next->process->priority >= process->priority) {
            current = current->next;
        }
        newNode->next = current->next;
        current->next = newNode;
    }
}

PCB* dequeue(PriorityQueueADT pq) {
    if (pq->first == NULL) {
        return NULL;
    }
    Node *temp = pq->first;
    PCB *process = temp->process;
    pq->first = pq->first->next;
    freeMemory(pq->memoryManager,temp);
    return process;
}

void checkPriorityQueue(PriorityQueueADT pq) {
    Node *current = pq->first;
    while (current != NULL) {
        if(current->next != NULL && current->process->priority < current->next->process->priority) {
            PCB *temp = current->process;
            current->process = current->next->process;
            current->next->process = temp;
        }
        current = current->next;
    }
}


PCB* removeFromQueue(PriorityQueueADT pq, PCB *process) {
    if (pq == NULL || process == NULL || pq->first == NULL) return NULL;
    
    if (pq->first->process == process) {
        Node *temp = pq->first;
        PCB *removedProcess = temp->process;
        pq->first = pq->first->next;
        freeMemory(pq->memoryManager, temp);
        return removedProcess;
    }

    Node *current = pq->first;
    while (current->next != NULL) {
        if (current->next->process == process) {
            Node *temp = current->next;
            PCB *removedProcess = temp->process;
            current->next = temp->next;
            freeMemory(pq->memoryManager, temp);
            return removedProcess;
        }
        current = current->next;
    }
    
    return NULL;
}

int isEmpty(PriorityQueueADT pq) {
    return (pq == NULL || pq->first == NULL);
}

PCB* peek(PriorityQueueADT pq) {
    if (pq == NULL || pq->first == NULL) return NULL;
    return pq->first->process;
}

int size(PriorityQueueADT pq) {
    if (pq == NULL) return 0;
    
    int count = 0;
    Node *current = pq->first;
    while (current != NULL) {
        count++;
        current = current->next;
    }
    return count;
}

void destroyPriorityQueue(PriorityQueueADT pq) {
    Node *current = pq->first;
    while (current != NULL) {
        Node *temp = current;
        current = current->next;
        freeMemory(pq->memoryManager, temp);
    }
    freeMemory(pq->memoryManager, pq);
}