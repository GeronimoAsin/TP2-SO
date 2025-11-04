#include "pipesList.h"

typedef struct PipeNode {
    Pipe* pipe;
    struct PipeNode* next;
} PipeNode;

struct PipeListCDT {
    PipeNode* head;
    PipeNode* tail;
    int size;
    MemoryManagerADT memoryManager;
};


PipeListADT createPipeList(MemoryManagerADT memoryManager) {
    PipeListADT pipeList = (PipeListADT)allocateMemory(memoryManager, sizeof(struct PipeListCDT));
    pipeList->head = NULL;
    pipeList->tail = NULL;
    pipeList->size = 0;
    pipeList->memoryManager = memoryManager;
    return pipeList;
}

void destroyPipeList(PipeListADT pipeList, MemoryManagerADT memoryManager) {
    PipeNode* current = pipeList->head;
    while (current != NULL) {
        PipeNode* toFree = current;
        current = current->next;
        freeMemory(memoryManager, toFree->pipe);
        freeMemory(memoryManager, toFree);
    }
    freeMemory(memoryManager, pipeList);
}

int addPipeToList(PipeListADT pipeList, Pipe* newPipe, MemoryManagerADT memoryManager) {
    PipeNode* newNode = (PipeNode*)allocateMemory(memoryManager, sizeof(PipeNode));
    if (!newNode) {
        return -1;
    }
    newNode->pipe = newPipe;
    newNode->next = NULL;

    if (pipeList->tail) {
        pipeList->tail->next = newNode;
    } else {
        pipeList->head = newNode;
    }
    pipeList->tail = newNode;
    pipeList->size++;
    return 0; 
}

Pipe* getPipeFromListToRead(PipeListADT pipeList, int read_fd) {
    PipeNode* current = pipeList->head;
    while (current != NULL) {
        if (current->pipe->read_fd == read_fd) {
            return current->pipe;
        }
        current = current->next;
    }
    return NULL; 
}

Pipe* getPipeFromListToWrite(PipeListADT pipeList, int write_fd) {
    PipeNode* current = pipeList->head;
    while (current != NULL) {
        if (current->pipe->write_fd == write_fd) {
            return current->pipe;
        }
        current = current->next;
    }
    return NULL; 
}

void deletePipe(PipeListADT pipeList, int read_fd, int write_fd) {
    PipeNode* current = pipeList->head;
    PipeNode* previous = NULL;

    while (current != NULL) {
        if (current->pipe->read_fd == read_fd && current->pipe->write_fd == write_fd) {
            if (previous) {
                previous->next = current->next;
            } else {
                pipeList->head = current->next;
            }
            if (current == pipeList->tail) {
                pipeList->tail = previous;
            }
            freeMemory(pipeList->memoryManager, current->pipe);
            freeMemory(pipeList->memoryManager, current);
            pipeList->size--;
            return;
        }
        previous = current;
        current = current->next;
    }
}