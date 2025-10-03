#ifndef LIST_H
#define LIST_H

#include <stdint.h>
typedef struct ListCDT * ListADT;
typedef int pid_t;

ListADT createList();

void addToList(ListADT list, PCB *process);

PCB* removeFromList(ListADT list, pid_t processId);

#endif // LIST_H