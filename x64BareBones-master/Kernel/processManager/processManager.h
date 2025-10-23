#ifndef PROCESS_MANAGER_H
#define PROCESS_MANAGER_H
#include <stdint.h>

typedef struct ProcessManagerCDT * ProcessManagerADT;
typedef int pid_t;
typedef unsigned int size_t;

typedef struct PCB {
    pid_t pid;
    pid_t parentPid;
    size_t priority;
    size_t state; //0 blocked, 1 ready, 2 running, 3 returned
    size_t foreground; //1 foreground, 0 background
    char *name;
    uint64_t *stackPointer;
    uint64_t *basePointer;
    uint64_t *instructionPointer;
    size_t stackSize;
    size_t argc;
    char **argv;
} PCB;

ProcessManagerADT createProcessManager();

pid_t createProcess(ProcessManagerADT pm, void (*entryPoint)(int, char**), int priority, char *name, int argc, char **argv);

pid_t getPid(ProcessManagerADT processManager);

void printProcesses(ProcessManagerADT processManager);

void kill(ProcessManagerADT processManager, pid_t processId);

void modifyPriority(ProcessManagerADT processManager, pid_t processId, int newPriority);

void block(ProcessManagerADT processManager, pid_t processId);

void unblock(ProcessManagerADT processManager, pid_t processId);

void leaveCPU(ProcessManagerADT processManager, pid_t processId);

void waitPid(ProcessManagerADT processManager, pid_t childPid);

void destroyProcessManager(ProcessManagerADT processManager);

#endif // PROCESS_MANAGER_H