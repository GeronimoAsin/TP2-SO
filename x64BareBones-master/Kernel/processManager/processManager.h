#ifndef PROCESS_MANAGER_H
#define PROCESS_MANAGER_H
#include <stdint.h>

typedef struct ProcessManagerCDT * ProcessManagerADT;
typedef int pid_t;

typedef struct PCB {
    pid_t pid;
    int priority;
    int state; //0 blocked, 1 ready, 2 running
    int foreground; //1 foreground, 0 background
    char *name;
    uint64_t *stackPointer;
    uint64_t *basePointer;
} PCB;

ProcessManagerADT createProcessManager();

void addProcess(ProcessManagerADT processManager, ...);

void removeProcess(ProcessManagerADT processManager, ...);

pid_t getPid(ProcessManagerADT processManager);

void printProcesses(ProcessManagerADT processManager);

void kill(ProcessManagerADT processManager, pid_t processId);

void modifyPriority(ProcessManagerADT processManager, pid_t processId, int newPriority);

void block(ProcessManagerADT processManager, pid_t processId);

void unblock(ProcessManagerADT processManager, pid_t processId);

void leaveCPU(ProcessManagerADT processManager, pid_t processId);

void waitPid(ProcessManagerADT processManager, pid_t processId);

void destroyProcessManager(ProcessManagerADT processManager);

#endif // PROCESS_MANAGER_H