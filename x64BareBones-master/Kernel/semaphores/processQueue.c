// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include "processQueue.h"

typedef struct ProcessNode {
    pid_t processId;
    struct ProcessNode* next;
} ProcessNode;

typedef struct ProcessQueueCDT {
    ProcessNode* head;
    ProcessNode* tail;
    MemoryManagerADT memoryManager;
} ProcessQueueCDT;

ProcessQueueADT createProcessQueue(MemoryManagerADT memoryManager) {
    ProcessQueueADT queue = (ProcessQueueADT)allocateMemory(memoryManager, sizeof(ProcessQueueCDT));
    queue->head = NULL;
    queue->tail = NULL;
    queue->memoryManager = memoryManager;
    return queue;
}

void destroyProcessQueue(ProcessQueueADT queue) {
    ProcessNode* current = queue->head;
    while (current != NULL) {
        ProcessNode* next = current->next;
        freeMemory(queue->memoryManager, current);
        current = next;
    }
    freeMemory(queue->memoryManager, queue);
}

void enqueueProcess(ProcessQueueADT queue, pid_t processId) {
    ProcessNode* newNode = (ProcessNode*)allocateMemory(queue->memoryManager, sizeof(ProcessNode));
    newNode->processId = processId;
    newNode->next = NULL;
    if (queue->tail != NULL) {
        queue->tail->next = newNode;
    }
    queue->tail = newNode;
    if (queue->head == NULL) {
        queue->head = newNode;
    }
}

pid_t dequeueProcess(ProcessQueueADT queue) {
    if (queue->head == NULL) {
        return -1;  
    }
    ProcessNode* temp = queue->head;
    pid_t processId = temp->processId;
    queue->head = temp->next;
    if (queue->head == NULL) {
        queue->tail = NULL;
    }
    freeMemory(queue->memoryManager, temp);
    return processId;
}
