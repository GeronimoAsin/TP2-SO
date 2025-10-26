#ifndef PROCESS_MANAGER_H
#define PROCESS_MANAGER_H

#include "pcb.h"
#include "../memoryManager/memoryManager.h"

typedef struct ProcessManagerCDT * ProcessManagerADT;

ProcessManagerADT createProcessManager(MemoryManagerADT memoryManager);

ProcessManagerADT getGlobalProcessManager(); // para usar en time.c

void createProcess(ProcessManagerADT pm, void (*entryPoint)(int, char**), int priority, char *name, int argc, char **argv);

pid_t getPid(ProcessManagerADT processManager);

void printProcesses(ProcessManagerADT processManager);

void kill(ProcessManagerADT processManager, pid_t processId);

void modifyPriority(ProcessManagerADT processManager, pid_t processId, int newPriority);

void block(ProcessManagerADT processManager, pid_t processId);

void unblock(ProcessManagerADT processManager, pid_t processId);

void leaveCPU(ProcessManagerADT processManager);

void waitPid(ProcessManagerADT processManager, pid_t childPid);

void destroyProcessManager(ProcessManagerADT processManager);

void context_switch(PCB *prev, PCB *next);

#endif // PROCESS_MANAGER_H
