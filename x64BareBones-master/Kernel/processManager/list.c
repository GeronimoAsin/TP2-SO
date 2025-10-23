#include "list.h"
#define NULL ((void*)0)

typedef struct Node {
    PCB *process;
    struct Node *next;
} Node;

typedef struct ListCDT {
    Node *head;
    Node *tail;
} ListCDT;

ListADT createList() {
    ListADT list = (ListADT) allocateMemory(sizeof(ListCDT));
    list->head = NULL;
    list->tail = NULL;
    return list;
}

void addToList(ListADT list, PCB *process) {
    Node *newNode = (Node *) allocateMemory(sizeof(Node));
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
            freeMemory(current);
            return process;
        }
        previous = current;
        current = current->next;
    }
    return NULL; // Process not found
}

void destroyList(ListADT list) {
    Node *current = list->head;
    while (current != NULL) {
        Node *next = current->next;
        freeMemory(current->process); 
        freeMemory(current);          
        current = next;
    }
    freeMemory(list);
}