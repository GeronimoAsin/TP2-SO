#include "processManager.h"
#include "list.h"
#include "priorityQueue.h"
#include "../include/time.h"
#define STACK_SIZE 0x4000 // 16 KiB

typedef struct ProcessManagerCDT {
    pid_t maxPid;
    pid_t currentPid;
    PriorityQueueADT readyQueue;
    ListADT allProcesses;
    ListADT blockedProcesses;
    MemoryManagerADT memoryManager;
    PCB * currentProcess;
} ProcessManagerCDT;

static ProcessManagerADT globalProcessManager = NULL;

ProcessManagerADT createProcessManager(MemoryManagerADT memoryManager) {
    ProcessManagerADT pm = (ProcessManagerADT) allocateMemory(memoryManager, sizeof(ProcessManagerCDT));
    pm->maxPid = 0;
    pm->currentPid = -1;
    pm->readyQueue = createPriorityQueue(memoryManager);
    pm->allProcesses = createList(memoryManager);
    pm->blockedProcesses = createList(memoryManager);
    pm->memoryManager = memoryManager;
    pm->currentProcess = NULL;
    globalProcessManager = pm;
    return pm;
}

ProcessManagerADT getGlobalProcessManager() {
    return globalProcessManager;
}

void createProcess(ProcessManagerADT pm, void (*entryPoint)(int, char**), int priority, char *name, int argc, char **argv){
    pm->maxPid += 1;
    PCB *newProcess = (PCB *) allocateMemory(pm->memoryManager, sizeof(PCB));
    
    newProcess->pid = pm->maxPid;
    newProcess->parentPid = -1; // No parent for now
    newProcess->priority = priority;
    newProcess->state = 1; // Ready state
    newProcess->foreground = 1; // Foreground process
    newProcess->name = name;
    newProcess->stackSize = STACK_SIZE;
    newProcess->stackBase = (uint64_t *) allocateMemory(pm->memoryManager, STACK_SIZE); //CHEQUEAR
    newProcess->stackPointer = newProcess->stackBase + (STACK_SIZE/sizeof(uint64_t)) - 1;  // Final del stack
    newProcess->basePointer = newProcess->stackPointer;  // Inicialmente iguales
    newProcess->argc = argc;
    newProcess->argv = argv;
    newProcess->instructionPointer = (uint64_t *) entryPoint;
    


    // Inicializar contexto básico
    memset(&newProcess->context, 0, sizeof(Context));
    newProcess->context.rip = (uint64_t)entryPoint;
    newProcess->context.rsp = (uint64_t)newProcess->stackPointer;
    newProcess->context.rbp = (uint64_t)newProcess->basePointer;
    newProcess->context.rflags = 0x202;  
    newProcess->context.cs = 0x08;       
    newProcess->context.ss = 0x00;       
    newProcess->context.rdi = argc;      
    newProcess->context.rsi = (uint64_t)argv;  
    
    enqueue(pm->readyQueue, newProcess);
    addToList(pm->allProcesses, newProcess);
    schedule();
}

pid_t getPid(ProcessManagerADT processManager) {
    return processManager->currentPid;
}

void printProcesses(ProcessManagerADT processManager) {
    // TODO: Implementar listado de procesos
    // Recorrer processManager->allProcesses e imprimir info de cada PCB
}

void kill(ProcessManagerADT processManager, pid_t processId) {
    PCB *process = findInList(processManager->allProcesses, processId);  
    if (process) {
        process->state = 3;  // TERMINATED
        removeFromQueue(processManager->readyQueue, process); 
        removeFromListByProcess(processManager->allProcesses, process);  
        freeMemory(processManager->memoryManager, process->stackBase);
        freeMemory(processManager->memoryManager, process);
        schedule();
    }
}

void modifyPriority(ProcessManagerADT processManager, pid_t processId, int newPriority) {
    PCB *process = findInList(processManager->allProcesses, processId);  
    if (process) {
        process->priority = newPriority;

        if (process->state == 1) {  // READY
            removeFromQueue(processManager->readyQueue, process); 
            enqueue(processManager->readyQueue, process);
        }
    }
}

void block(ProcessManagerADT processManager, pid_t processId) {
    PCB *process = findInList(processManager->allProcesses, processId);  
    if (process && process->state != 0) {
        int oldState = process->state;  // Guardar estado anterior
        process->state = 0;  // BLOCKED
        
        if (oldState == 1) {  // Usar estado anterior
            removeFromQueue(processManager->readyQueue, process); 
        }
        
        addToList(processManager->blockedProcesses, process);  
        schedule();
    }
}

void unblock(ProcessManagerADT processManager, pid_t processId) {
    PCB *process = findInList(processManager->blockedProcesses, processId);  
    if (process && process->state == 0) {
        process->state = 1;  // READY
        removeFromListByProcess(processManager->blockedProcesses, process);
        enqueue(processManager->readyQueue, process);
    }
}

void leaveCPU(ProcessManagerADT processManager) {
    PCB *current = processManager->currentProcess;
    if (current && current->state == 2) {  // RUNNING
        current->state = 1;  // READY
        enqueue(processManager->readyQueue, current);
        // No llamar scheduler_tick aquí - es solo para timer interrupt
    }
}

void waitPid(ProcessManagerADT processManager, pid_t childPid) {
    PCB *child = findInList(processManager->allProcesses, childPid);  
    if (!child) return;
    
    if (child->state != 3) {  
        block(processManager, processManager->currentPid);  
    }
}

void destroyProcessManager(ProcessManagerADT processManager) {
    destroyPriorityQueue(processManager->readyQueue);
    destroyList(processManager->allProcesses);
    freeMemory(processManager->memoryManager, processManager);
}

void saveContextToPCB(PCB *pcb, uint64_t *savedContext) {
    pcb->context.rax = savedContext[0];
    pcb->context.rbx = savedContext[1];  
    pcb->context.rcx = savedContext[2];
    pcb->context.rdx = savedContext[3];
    pcb->context.rsi = savedContext[4];
    pcb->context.rdi = savedContext[5];
    pcb->context.rsp = savedContext[6];
    pcb->context.rbp = savedContext[7];
    pcb->context.r8 = savedContext[8];
    pcb->context.r9 = savedContext[9];
    pcb->context.r10 = savedContext[10];
    pcb->context.r11 = savedContext[11];
    pcb->context.r12 = savedContext[12];
    pcb->context.r13 = savedContext[13];
    pcb->context.r14 = savedContext[14];
    pcb->context.r15 = savedContext[15];
    pcb->context.rip = savedContext[16];    
    pcb->context.cs = savedContext[17];     
    pcb->context.rflags = savedContext[18];
    pcb->context.ss = savedContext[19];     
    pcb->context.align = savedContext[20];  
}

void loadContextFromPCB(PCB *pcb, uint64_t *savedContext) {
    savedContext[0] = pcb->context.rax;
    savedContext[1] = pcb->context.rbx;
    savedContext[2] = pcb->context.rcx;
    savedContext[3] = pcb->context.rdx;
    savedContext[4] = pcb->context.rsi;
    savedContext[5] = pcb->context.rdi;
    savedContext[6] = pcb->context.rsp;
    savedContext[7] = pcb->context.rbp;
    savedContext[8] = pcb->context.r8;
    savedContext[9] = pcb->context.r9;
    savedContext[10] = pcb->context.r10;
    savedContext[11] = pcb->context.r11;
    savedContext[12] = pcb->context.r12;
    savedContext[13] = pcb->context.r13;
    savedContext[14] = pcb->context.r14;
    savedContext[15] = pcb->context.r15;
    savedContext[16] = pcb->context.rip;
    savedContext[17] = pcb->context.cs; 
    savedContext[18] = pcb->context.rflags;
    savedContext[19] = pcb->context.ss; 
    savedContext[20] = pcb->context.align;
}


void scheduler_tick(ProcessManagerADT pm, uint64_t *savedContext) {
    PCB *currentProcess = pm->currentProcess;
    if (currentProcess) {
        saveContextToPCB(currentProcess, savedContext);
    }
    
    PCB *nextProcess = dequeue(pm->readyQueue);
    if (!nextProcess) {
        return;
    }
    
    if (nextProcess != currentProcess) {
        if (currentProcess) {
            currentProcess->state = 1; // READY
            enqueue(pm->readyQueue, currentProcess); // Volver a la cola
        }
        
        nextProcess->state = 2; // RUNNING
        pm->currentProcess = nextProcess;
        pm->currentPid = nextProcess->pid;
        loadContextFromPCB(nextProcess, savedContext);
    }
    
}