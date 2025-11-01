#ifndef PROCESS_MANAGER_H
#define PROCESS_MANAGER_H

#include "pcb.h"
#include "../memoryManager/memoryManager.h"
#include "priorityQueue.h"

typedef struct ProcessManagerCDT * ProcessManagerADT;

ProcessManagerADT createProcessManager(MemoryManagerADT memoryManager);

ProcessManagerADT getGlobalProcessManager(); // para usar en time.c

pid_t createProcess(ProcessManagerADT pm, void (*entryPoint)(int, char**), int priority, char *name, int argc, char **argv, int foreground);

pid_t getPid(ProcessManagerADT processManager);

void printProcesses(ProcessManagerADT processManager);

void kill(ProcessManagerADT processManager, pid_t processId);

void modifyPriority(ProcessManagerADT processManager, pid_t processId, int newPriority);

void block(ProcessManagerADT processManager, pid_t processId);

void unblock(ProcessManagerADT processManager, pid_t processId);

void leaveCPU(ProcessManagerADT processManager);

void waitPid(ProcessManagerADT processManager, pid_t childPid);

pid_t fg(ProcessManagerADT processManager);

void destroyProcessManager(ProcessManagerADT processManager);

void context_switch(PCB *prev, PCB *next);

pid_t getMaxPid(ProcessManagerADT processManager);
PriorityQueueADT getReadyQueue(ProcessManagerADT processManager);
PCB* getCurrentProcess(ProcessManagerADT processManager);
void clearAllProcesses(ProcessManagerADT processManager);

// Context switch functions
uint64_t schedule(uint64_t current_rsp);

#endif // PROCESS_MANAGER_H
