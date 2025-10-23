#include "processManager.h"
#include "list.h"
#include "priorityQueue.h"
#define STACK_SIZE 0x4000 // 16 KiB

typedef struct ProcessManagerCDT {
    pid_t maxPid;
    pid_t currentPid;
    PriorityQueueADT readyQueue;
    ListADT allProcesses;
} ProcessManagerCDT;

ProcessManagerADT createProcessManager() {
    ProcessManagerADT pm = (ProcessManagerADT) allocateMemory(sizeof(ProcessManagerCDT));
    pm->maxPid = -1;
    pm->currentPid = -1;
    pm->readyQueue = createPriorityQueue();
    pm->allProcesses = createList();
    return pm;
}

pid_t createProcess(ProcessManagerADT pm, void (*entryPoint)(int, char**), int priority, char *name, int argc, char **argv){
    pm->maxPid += 1;
    PCB *newProcess = (PCB *) allocateMemory(sizeof(PCB));
    newProcess->pid = pm->maxPid;
    newProcess->parentPid = -1; // No parent for now
    newProcess->priority = priority;
    newProcess->state = 1; // Ready state
    newProcess->foreground = 1; // Foreground process
    newProcess->name = name;
    newProcess->stackSize = STACK_SIZE;
    newProcess->stackPointer = (uint64_t *) allocateMemory(STACK_SIZE);
    newProcess->basePointer = newProcess->stackPointer + STACK_SIZE;
    newProcess->argc = argc;
    newProcess->argv = argv;
    newProcess->instructionPointer = (uint64_t *) entryPoint;
    enqueue(pm->readyQueue, newProcess);
    addToList(pm->allProcesses, newProcess);
    return newProcess->pid;
}

pid_t getPid(ProcessManagerADT processManager) {
    return processManager->currentPid;
}

void printProcesses(ProcessManagerADT processManager) {
    // Implementation to print all processes
}

void kill(ProcessManagerADT processManager, pid_t processId) {
    PCB *process = removeFromList(processManager->allProcesses, processId);
    if (process) {
        process->state = 3; 
        addToList(processManager->readyQueue, process);
    }
}

void modifyPriority(ProcessManagerADT processManager, pid_t processId, int newPriority) {
    PCB *process = removeFromList(processManager->allProcesses, processId);
    if (process) {
        process->priority = newPriority;
        addToList(processManager->allProcesses, process);
        checkPriorityQueue(processManager->readyQueue);
    }
}

void block(ProcessManagerADT processManager, pid_t processId) {
    PCB *process = removeFromList(processManager->allProcesses, processId);
    if (process) {
        process->state = 0; 
        addToList(processManager->allProcesses, process);
    }
}

void unblock(ProcessManagerADT processManager, pid_t processId) {
    PCB *process = removeFromList(processManager->allProcesses, processId);
    if (process) {
        process->state = 1; 
        enqueue(processManager->readyQueue, process);
        addToList(processManager->allProcesses, process);
    }
}

void leaveCPU(ProcessManagerADT processManager, pid_t processId) {
    PCB *process = removeFromList(processManager->allProcesses, processId);
    if (process) {
        process->state = 1; 
        enqueue(processManager->readyQueue, process);
        addToList(processManager->allProcesses, process);
    }
}

void waitPid(ProcessManagerADT processManager, pid_t childPid) {
    PCB *process = removeFromList(processManager->allProcesses, childPid);
    if(process && process->state != 3) {
        pid_t myPid = processManager->currentPid;
        block(processManager, myPid);
    }
    removeFromList(processManager->allProcesses, childPid);
}

void destroyProcessManager(ProcessManagerADT processManager) {
    destroyPriorityQueue(processManager->readyQueue);
    destroyList(processManager->allProcesses);
    freeMemory(processManager);
}