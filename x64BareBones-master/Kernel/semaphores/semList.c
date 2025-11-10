// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include "semList.h"
#include "../memoryManager/memoryManager.h"
#include "../include/lib.h"


typedef struct SemNode {
    Semaphore *semaphore;
    struct SemNode *next;
} SemNode;

typedef struct SemListCDT {
    SemNode *head;
    SemNode *tail;
    MemoryManagerADT memoryManager;
} SemListCDT;

SemListADT createSemList(MemoryManagerADT memoryManager) {
    SemListADT list = (SemListADT) allocateMemory(memoryManager, sizeof(SemListCDT));
    list->head = NULL;
    list->tail = NULL;
    list->memoryManager = memoryManager;
    return list;
}

void addSemToList(SemListADT list, Semaphore *semaphore) {
    SemNode *newNode = (SemNode *) allocateMemory(list->memoryManager, sizeof(SemNode));
    newNode->semaphore = semaphore;
    newNode->next = NULL;

    if (list->tail == NULL) {
        list->head = newNode;
        list->tail = newNode;
    } else {
        list->tail->next = newNode;
        list->tail = newNode;
    }
}

Semaphore* removeFromSemList(SemListADT list, sem_id semaphoreId) {
    SemNode *current = list->head;
    SemNode *previous = NULL;

    while (current != NULL) {
        if (strcmp(current->semaphore->id, semaphoreId) == 0) {
            if (previous == NULL) {
                list->head = current->next;
            } else {
                previous->next = current->next;
            }
            if (current == list->tail) {
                list->tail = previous;
            }
            Semaphore *semaphore = current->semaphore;
            freeMemory(list->memoryManager,current);
            return semaphore;
        }
        previous = current;
        current = current->next;
    }
    return NULL; 
}


Semaphore* findInSemList(SemListADT list, sem_id semaphoreId) {
    if (list == NULL) return NULL;
    
    SemNode *current = list->head;
    while (current != NULL) {
        if (strcmp(current->semaphore->id, semaphoreId) == 0) {
            return current->semaphore;
        }
        current = current->next;
    }
    return NULL; 
}

int isInSemList(SemListADT list, sem_id semaphoreId) {
    return findInSemList(list, semaphoreId) != NULL;
}

int semListSize(SemListADT list) {
    if (list == NULL) return 0;
    
    int size = 0;
    SemNode *current = list->head;
    while (current != NULL) {
        size++;
        current = current->next;
    }
    return size;
}

Semaphore* getAtSem(SemListADT list, int index) {
    if (list == NULL || index < 0) return NULL;
    
    SemNode *current = list->head;
    int currentIndex = 0;
    
    while (current != NULL && currentIndex < index) {
        current = current->next;
        currentIndex++;
    }
    
    return (current != NULL) ? current->semaphore : NULL;
}

void removeFromSemListByProcess(SemListADT list, Semaphore *semaphore) {
    if (list == NULL || semaphore == NULL) return;

    SemNode *current = list->head;
    SemNode *previous = NULL;

    while (current != NULL) {
        if (current->semaphore == semaphore) {
            if (previous == NULL) {
                list->head = current->next;
            } else {
                previous->next = current->next;
            }
            if (current == list->tail) {
                list->tail = previous;
            }
            freeMemory(list->memoryManager, current);
            return;
        }
        previous = current;
        current = current->next;
    }
}

void clearSemList(SemListADT list) {
    if (list == NULL) return;
    
    SemNode *current = list->head;
    while (current != NULL) {
        SemNode *next = current->next;
        freeMemory(list->memoryManager, current);
        current = next;
    }
    list->head = NULL;
    list->tail = NULL;
}

int isEmptySemList(SemListADT list) {
    return (list == NULL || list->head == NULL);
}

Semaphore* removeFirstFromSemList(SemListADT list) {
    if (list == NULL || list->head == NULL) return NULL;
    SemNode *first = list->head;
    Semaphore *sem = first->semaphore;
    list->head = first->next;
    if (list->head == NULL) list->tail = NULL;
    freeMemory(list->memoryManager, first);
    return sem;
}

void destroySemList(SemListADT list) {
    SemNode *current = list->head;
    while (current != NULL) {
        SemNode *next = current->next;
        // NO liberamos current->semaphore aquí, eso se hace en sem_close
        freeMemory(list->memoryManager, current);
        current = next;
    }
    freeMemory(list->memoryManager, list);
}

