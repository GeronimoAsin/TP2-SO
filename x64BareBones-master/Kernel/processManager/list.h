#ifndef LIST_H
#define LIST_H

#include "pcb.h"

typedef struct ListCDT * ListADT;

ListADT createList();

void addToList(ListADT list, PCB *process);

PCB* removeFromList(ListADT list, pid_t processId);

void destroyList(ListADT list);

#endif // LIST_H