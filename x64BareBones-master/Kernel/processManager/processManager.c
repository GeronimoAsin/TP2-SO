#include "processManager.h"
#include "list.h"
#include "priorityQueue.h"
#include "../include/time.h"
#include "../include/lib.h"
#include <string.h>

extern void schedules();
extern uint64_t fill_stack(uint64_t stack_top, uint64_t entry_point, uint64_t argc, uint64_t * argv);
extern void idle();

// Stack size para procesos: debe ser <= 4096 bytes (tamaño de chunk del memory manager)
#define PROCESS_STACK_SIZE 4096

typedef struct ProcessManagerCDT {
    pid_t maxPid;
    pid_t currentPid;
    PriorityQueueADT readyQueue;
    ListADT allProcesses;
    ListADT blockedProcesses;
    MemoryManagerADT memoryManager;
    PCB * currentProcess;
    PCB * idleProcess;
} ProcessManagerCDT;

static ProcessManagerADT globalProcessManager = NULL;

ProcessManagerADT createProcessManager(MemoryManagerADT memoryManager) {
    ProcessManagerADT pm = (ProcessManagerADT) allocateMemory(memoryManager, sizeof(ProcessManagerCDT));
    pm->maxPid = 0;
    pm->currentPid = 0;
    pm->readyQueue = createPriorityQueue(memoryManager);
    pm->allProcesses = createList(memoryManager);
    pm->blockedProcesses = createList(memoryManager);
    pm->memoryManager = memoryManager;
    pm->currentProcess = NULL;
    pm->idleProcess = NULL;
    globalProcessManager = pm;
    
    // Proceso idle
    PCB *idleProc = (PCB *) allocateMemory(memoryManager, sizeof(PCB));
    idleProc->pid = 0;
    idleProc->parentPid = 0;
    idleProc->priority = 0;
    idleProc->state = 1; // READY
    idleProc->foreground = 0;
    idleProc->name = "Idle";
    idleProc->stackSize = PROCESS_STACK_SIZE;
    idleProc->stackBase = (uint64_t *) allocateMemory(memoryManager, PROCESS_STACK_SIZE);
    idleProc->argc = 0;
    idleProc->argv = NULL;
    idleProc->read_fd = 0;
    idleProc->write_fd = 1;

    uint64_t idle_stack_top = (uint64_t)(idleProc->stackBase) + PROCESS_STACK_SIZE;
    idleProc->stackPointer = (uint64_t *)fill_stack(idle_stack_top, (uint64_t)idle, 0, 0);
    
    pm->idleProcess = idleProc;
    
    addToList(pm->allProcesses, idleProc);
    
    return pm;
}

ProcessManagerADT getGlobalProcessManager() {
    return globalProcessManager;
}

pid_t createProcess(ProcessManagerADT pm, void (*entryPoint)(int, char**), int priority, char *name, int argc, char **argv, int foreground){
    pm->maxPid += 1;
    PCB *newProcess = (PCB *) allocateMemory(pm->memoryManager, sizeof(PCB));
    
    newProcess->pid = pm->maxPid;
    newProcess->parentPid = (pm->currentProcess != NULL) ? pm->currentProcess->pid : 0;
    newProcess->priority = priority;
    newProcess->state = 1; 
    newProcess->foreground = foreground;
    newProcess->name = name;
    newProcess->stackSize = PROCESS_STACK_SIZE;
    newProcess->stackBase = (uint64_t *) allocateMemory(pm->memoryManager, newProcess->stackSize);
    newProcess->read_fd = 0; 
    newProcess->write_fd = 1; 
    uint64_t stack_top = (uint64_t)(newProcess->stackBase) + PROCESS_STACK_SIZE;

    newProcess->argc = argc;
    newProcess->argv = argv;
    newProcess->instructionPointer = (uint64_t *) entryPoint;
    uint64_t new_rsp = fill_stack(stack_top, (uint64_t)entryPoint, (uint64_t)argc, (uint64_t *)argv);

    newProcess->stackPointer = (uint64_t *)new_rsp;
    newProcess->basePointer = (uint64_t *)new_rsp;
    
    enqueue(pm->readyQueue, newProcess);
    addToList(pm->allProcesses, newProcess);
    schedules();
    return newProcess->pid;
}

pid_t createProcessWithFds(ProcessManagerADT pm, void (*entryPoint)(int, char**), int priority, char *name, int argc, char **argv, int foreground, int initial_read_fd, int initial_write_fd){
    pm->maxPid += 1;
    PCB *newProcess = (PCB *) allocateMemory(pm->memoryManager, sizeof(PCB));

    newProcess->pid = pm->maxPid;
    newProcess->parentPid = (pm->currentProcess != NULL) ? pm->currentProcess->pid : 0;
    newProcess->priority = priority;
    newProcess->state = 1; 
    newProcess->foreground = foreground; 
    newProcess->name = name;
    newProcess->stackSize = PROCESS_STACK_SIZE;
    newProcess->stackBase = (uint64_t *) allocateMemory(pm->memoryManager, newProcess->stackSize);
    newProcess->read_fd = (initial_read_fd >= 0) ? initial_read_fd : 0;
    newProcess->write_fd = (initial_write_fd >= 0) ? initial_write_fd : 1;

    uint64_t stack_top = (uint64_t)(newProcess->stackBase) + PROCESS_STACK_SIZE;
    newProcess->argc = argc;
    newProcess->argv = argv;
    newProcess->instructionPointer = (uint64_t *) entryPoint;

    uint64_t new_rsp = fill_stack(stack_top, (uint64_t)entryPoint, (uint64_t)argc, (uint64_t *)argv);

    newProcess->stackPointer = (uint64_t *)new_rsp;
    newProcess->basePointer = (uint64_t *)new_rsp;

    enqueue(pm->readyQueue, newProcess);
    addToList(pm->allProcesses, newProcess);
    schedules();
    return newProcess->pid;
}

void setWriteFd(ProcessManagerADT processManager, pid_t processId, int write_fd) {
    PCB *process = findInList(processManager->allProcesses, processId);  
    if (process) {
        process->write_fd = write_fd;
    }
}

void setReadFd(ProcessManagerADT processManager, pid_t processId, int read_fd) {
    PCB *process = findInList(processManager->allProcesses, processId);  
    if (process) {
        process->read_fd = read_fd;
    }
}

int getWriteFd(ProcessManagerADT processManager, pid_t processId) {
    PCB *process = findInList(processManager->allProcesses, processId);  
    if (process) {
        return process->write_fd;
    }
    return -1; 
}

int getReadFd(ProcessManagerADT processManager, pid_t processId) {
    PCB *process = findInList(processManager->allProcesses, processId);  
    if (process) {
        return process->read_fd;
    }
    return -1; 
}

pid_t getPid(ProcessManagerADT processManager) {
    return processManager->currentPid;
}

pid_t getMaxPid(ProcessManagerADT processManager) {
    return processManager->maxPid;
}

PriorityQueueADT getReadyQueue(ProcessManagerADT processManager) {
    return processManager->readyQueue;
}

PCB* getCurrentProcess(ProcessManagerADT processManager) {
    return processManager->currentProcess;
}

void clearAllProcesses(ProcessManagerADT processManager) {
    while (!isListEmpty(processManager->allProcesses)) {
        PCB *proc = removeFirstFromList(processManager->allProcesses);
        if (proc) {
            freeMemory(processManager->memoryManager, proc->stackBase);
            freeMemory(processManager->memoryManager, proc);
        }
    }
    while (!isEmpty(processManager->readyQueue)) {
        dequeue(processManager->readyQueue);
    }
    while (!isListEmpty(processManager->blockedProcesses)) {
        removeFirstFromList(processManager->blockedProcesses);
    }
    processManager->maxPid = 0;
    processManager->currentPid = 0;
    processManager->currentProcess = NULL;
}




ProcessInfo * getProcesses(ProcessManagerADT processManager, size_t *outCount) {
    size_t count = listSize(processManager->allProcesses);
    *outCount = count; 
    
    ProcessInfo *toReturn = (ProcessInfo *) allocateMemory(processManager->memoryManager, sizeof(ProcessInfo) * count);
    for(int i = 0; i < count; i++) {
        PCB *proc = getAt(processManager->allProcesses, i);
        toReturn[i].pid = proc->pid;
        toReturn[i].name = proc->name;
        toReturn[i].state = proc->state;
        toReturn[i].priority = proc->priority;
        toReturn[i].stackPointer = proc->stackPointer;
        toReturn[i].parentPid = proc->parentPid;
        toReturn[i].foreground = proc->foreground;
    }
    return toReturn;
}

void kill(ProcessManagerADT processManager, pid_t processId) {
    PCB *process = findInList(processManager->allProcesses, processId);  
    if (process) {
        process->state = 3; 
        removeFromQueue(processManager->readyQueue, process);
        
        if (process->parentPid > 0) {
            PCB *parent = findInList(processManager->allProcesses, process->parentPid);
            if (parent && parent->state == 0) {  
                unblock(processManager, process->parentPid);
            }
        }
        schedules();
    }
}

void modifyPriority(ProcessManagerADT processManager, pid_t processId, int newPriority) {
    PCB *process = findInList(processManager->allProcesses, processId);  
    if (process) {
        process->priority = newPriority;

        if (process->state == 1) {  
            removeFromQueue(processManager->readyQueue, process); 
            enqueue(processManager->readyQueue, process);
        }
    }
}

void block(ProcessManagerADT processManager, pid_t processId) {
    PCB *process = findInList(processManager->allProcesses, processId);  
    if (process == NULL) {
        return;
    }

    if (process->state == 0 || process->state == 3) { 
        return;
    }

    int oldState = process->state;  
    process->state = 0;  

    if (oldState == 1) {  
        removeFromQueue(processManager->readyQueue, process); 
    }

    addToList(processManager->blockedProcesses, process);
    schedules();
}

void unblock(ProcessManagerADT processManager, pid_t processId) {
    PCB *process = findInList(processManager->blockedProcesses, processId);  
    if (!process || process->state != 0) {
        return;
    }

    process->state = 1;  
    removeFromListByProcess(processManager->blockedProcesses, process);
    enqueue(processManager->readyQueue, process);
}

void leaveCPU(ProcessManagerADT processManager) {
    PCB *current = processManager->currentProcess;
    if (current && current->state == 2) {  
        current->state = 1;  
        enqueue(processManager->readyQueue, current);
    }
}

void waitPid(ProcessManagerADT processManager, pid_t childPid) {
    PCB *child = findInList(processManager->allProcesses, childPid);  
    if (!child) {
        return;
    }

    while (child->state != 3) {  
        block(processManager, processManager->currentPid);
        
        child = findInList(processManager->allProcesses, childPid);
        if (!child) {
            return;
        }
    }
    
    removeFromListByProcess(processManager->allProcesses, child);
    freeMemory(processManager->memoryManager, child->stackBase);
    freeMemory(processManager->memoryManager, child);
}


pid_t fg(ProcessManagerADT processManager) {
    for (int i = 0; i < listSize(processManager->allProcesses); i++) {
        PCB *proc = getAt(processManager->allProcesses, i);
        if (proc->foreground == 0 && (proc->state == 1 || proc->state == 2)) {
            proc->foreground = 1;

            if (processManager->currentProcess) {
                waitPid(processManager, proc->pid);
            }
            
            return proc->pid;
        }
    }
    
    return -1; 
}

void destroyProcessManager(ProcessManagerADT processManager) {
    destroyPriorityQueue(processManager->readyQueue);
    destroyList(processManager->allProcesses);
    freeMemory(processManager->memoryManager, processManager);
}


uint64_t schedule(uint64_t current_rsp) {
    ProcessManagerADT pm = getGlobalProcessManager();
    if (!pm) {
        return current_rsp; 
    }

    PCB *currentProcess = pm->currentProcess;

    if (currentProcess) {
        currentProcess->stackPointer = (uint64_t *)current_rsp;
        
        if (currentProcess->state == 2) { 
            currentProcess->state = 1; 
            enqueue(pm->readyQueue, currentProcess);
        } 
    }
    

    PCB *nextProcess = dequeue(pm->readyQueue);
    if (!nextProcess) {
        nextProcess = pm->idleProcess;
    }

    nextProcess->state = 2; 
    pm->currentProcess = nextProcess;
    pm->currentPid = nextProcess->pid;

    return (uint64_t)nextProcess->stackPointer;
}

void exitProcess(ProcessManagerADT pm, pid_t processId) {
    PCB *process = findInList(pm->allProcesses, processId);  
    if (process) {
        process->state = 3; 
        removeFromQueue(pm->readyQueue, process);
        
        if (process->parentPid > 0) {
            PCB *parent = findInList(pm->allProcesses, process->parentPid);
            if (parent && parent->state == 0) {  
                unblock(pm, process->parentPid);
            }
        }

    }
    schedules(); 
}