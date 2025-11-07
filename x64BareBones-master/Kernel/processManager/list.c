#include "list.h"
#include "../memoryManager/memoryManager.h"
#define NULL ((void*)0)

typedef struct Node {
    PCB *process;
    struct Node *next;
} Node;

typedef struct ListCDT {
    Node *head;
    Node *tail;
    MemoryManagerADT memoryManager;
} ListCDT;

ListADT createList(MemoryManagerADT memoryManager) {
    ListADT list = (ListADT) allocateMemory(memoryManager, sizeof(ListCDT));
    list->head = NULL;
    list->tail = NULL;
    list->memoryManager = memoryManager;
    return list;
}

void addToList(ListADT list, PCB *process) {
    Node *newNode = (Node *) allocateMemory(list->memoryManager, sizeof(Node));
    newNode->process = process;
    newNode->next = NULL;

    if (list->tail == NULL) {
        list->head = newNode;
        list->tail = newNode;
    } else {
        list->tail->next = newNode;
        list->tail = newNode;
    }
}

PCB* removeFromList(ListADT list, pid_t processId) {
    Node *current = list->head;
    Node *previous = NULL;

    while (current != NULL) {
        if (current->process->pid == processId) {
            if (previous == NULL) {
                list->head = current->next;
            } else {
                previous->next = current->next;
            }
            if (current == list->tail) {
                list->tail = previous;
            }
            PCB *process = current->process;
            freeMemory(list->memoryManager,current);
            return process;
        }
        previous = current;
        current = current->next;
    }
    return NULL; 
}


PCB* findInList(ListADT list, pid_t processId) {
    if (list == NULL) return NULL;
    
    Node *current = list->head;
    while (current != NULL) {
        if (current->process->pid == processId) {
            return current->process;
        }
        current = current->next;
    }
    return NULL; 
}

int isInList(ListADT list, pid_t processId) {
    return findInList(list, processId) != NULL;
}

int listSize(ListADT list) {
    if (list == NULL) return 0;
    
    int size = 0;
    Node *current = list->head;
    while (current != NULL) {
        size++;
        current = current->next;
    }
    return size;
}

PCB* getAt(ListADT list, int index) {
    if (list == NULL || index < 0) return NULL;
    
    Node *current = list->head;
    int currentIndex = 0;
    
    while (current != NULL && currentIndex < index) {
        current = current->next;
        currentIndex++;
    }
    
    return (current != NULL) ? current->process : NULL;
}

void removeFromListByProcess(ListADT list, PCB *process) {
    if (list == NULL || process == NULL) return;
    
    Node *current = list->head;
    Node *previous = NULL;

    while (current != NULL) {
        if (current->process == process) {
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

void clearList(ListADT list) {
    if (list == NULL) return;
    
    Node *current = list->head;
    while (current != NULL) {
        Node *next = current->next;
        freeMemory(list->memoryManager, current);
        current = next;
    }
    list->head = NULL;
    list->tail = NULL;
}

int isEmptyList(ListADT list) {
    return (list == NULL || list->head == NULL);
}

PCB* removeFirstFromList(ListADT list) {
    if (list == NULL || list->head == NULL) return NULL;
    Node *first = list->head;
    PCB *proc = first->process;
    list->head = first->next;
    if (list->head == NULL) list->tail = NULL;
    freeMemory(list->memoryManager, first);
    return proc;
}

int isListEmpty(ListADT list) {
    return (list == NULL || list->head == NULL);
}

void destroyList(ListADT list) {
    Node *current = list->head;
    while (current != NULL) {
        Node *next = current->next;
        freeMemory(list->memoryManager, current->process); 
        freeMemory(list->memoryManager, current);
        current = next;
    }
    freeMemory(list->memoryManager, list);
}

PCB * findFirstWaitingToRead(ListADT list) {
    if (list == NULL) return NULL;
    
    Node *current = list->head;
    while (current != NULL) {
        current = current->next;
    }
    return NULL; 
}