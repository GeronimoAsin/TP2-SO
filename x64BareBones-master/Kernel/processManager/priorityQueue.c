#include "priorityQueue.h"
#define NULL ((void*)0)

typedef struct Node {
    PCB *process;
    struct Node *next;
} Node;

typedef struct PriorityQueueCDT {
    Node *first;
} PriorityQueueCDT;

PriorityQueueADT createPriorityQueue() {
    PriorityQueueADT pq = (PriorityQueueADT) allocateMemory(sizeof(PriorityQueueCDT));
    pq->first = NULL;
    return pq;
}

void enqueue(PriorityQueueADT pq, PCB *process) {
    Node *newNode = (Node *) allocateMemory(sizeof(Node));
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
    freeMemory(temp);
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

void destroyPriorityQueue(PriorityQueueADT pq) {
    Node *current = pq->first;
    while (current != NULL) {
        Node *temp = current;
        current = current->next;
        freeMemory(temp);
    }
    freeMemory(pq);
}