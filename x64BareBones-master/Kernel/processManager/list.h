#ifndef LIST_H
#define LIST_H

#include "../memoryManager/memoryManager.h"
#include "pcb.h"

typedef struct ListCDT * ListADT;

ListADT createList(MemoryManagerADT memoryManager);

void addToList(ListADT list, PCB *process);

PCB* removeFromList(ListADT list, pid_t processId);

PCB* findInList(ListADT list, pid_t processId);  

int isInList(ListADT list, pid_t processId);  

int listSize(ListADT list);  

PCB* getAt(ListADT list, int index);  

void removeFromListByProcess(ListADT list, PCB *process); 

void clearList(ListADT list);

int isEmptyList(ListADT list);

void destroyList(ListADT list);

PCB* removeFirstFromList(ListADT list);



#endif // LIST_H