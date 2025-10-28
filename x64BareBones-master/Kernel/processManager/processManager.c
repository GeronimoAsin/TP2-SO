#include "processManager.h"
#include "list.h"
#include "priorityQueue.h"
#include "../include/time.h"
#include "../include/lib.h"
#include <string.h>

static void printHex64(uint64_t value)
{
    char buf[19]; // "0x" + 16 hex + '\0'
    buf[0] = '0';
    buf[1] = 'x';
    for (int i = 0; i < 16; i++)
    {
        uint8_t nibble = (value >> ((15 - i) * 4)) & 0xF;
        buf[2 + i] = (nibble < 10) ? ('0' + nibble) : ('A' + (nibble - 10));
    }
    buf[18] = '\0';
    printString(buf);
}

extern void schedules();
extern uint64_t fill_stack(uint64_t stack_top, uint64_t entry_point, uint64_t argc, uint64_t argv);

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
} ProcessManagerCDT;

static ProcessManagerADT globalProcessManager = NULL;

ProcessManagerADT createProcessManager(MemoryManagerADT memoryManager) {
    ProcessManagerADT pm = (ProcessManagerADT) allocateMemory(memoryManager, sizeof(ProcessManagerCDT));
    printHex64(pm);
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
    newProcess->stackSize = PROCESS_STACK_SIZE;
    newProcess->stackBase = (uint64_t *) allocateMemory(pm->memoryManager, newProcess->stackSize);

    // Calcular el tope del stack (crece hacia abajo)
    uint64_t stack_top = (uint64_t)(newProcess->stackBase) + PROCESS_STACK_SIZE;

    printHex64(stack_top);
    newLine();
    printHex64(newProcess->stackBase);
    newLine();

    newProcess->argc = argc;
    newProcess->argv = argv;
    newProcess->instructionPointer = (uint64_t *) entryPoint;
    
    printString("Entry point: ");
    printHex64((uint64_t)entryPoint);
    newLine();

    // Usar fill_stack para inicializar el stack del proceso
    uint64_t new_rsp = fill_stack(stack_top, (uint64_t)entryPoint, (uint64_t)argc, (uint64_t)argv);

    printString("new_rsp returned: ");
    printHex64(new_rsp);
    newLine();

    newProcess->stackPointer = (uint64_t *)new_rsp;
    newProcess->basePointer = (uint64_t *)new_rsp;

    // Inicializar contexto básico
    memset(&newProcess->context, 0, sizeof(Context));
    newProcess->context.rip = (uint64_t)entryPoint;
    newProcess->context.rsp = new_rsp;
    newProcess->context.rbp = new_rsp;
    newProcess->context.rflags = 0x202;
    newProcess->context.cs = 0x08;       
    newProcess->context.ss = 0x00;       
    newProcess->context.rdi = argc;      
    newProcess->context.rsi = (uint64_t)argv;  
    
    enqueue(pm->readyQueue, newProcess);
    addToList(pm->allProcesses, newProcess);
    schedules();
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
    processManager->currentPid = -1;
    processManager->currentProcess = NULL;
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
        schedules();
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
        schedules();
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

// Nueva función schedule que maneja el context switch
uint64_t schedule(uint64_t current_rsp) {
    ProcessManagerADT pm = getGlobalProcessManager();
    if (!pm) {
        return current_rsp; // No hay process manager, continuar con el proceso actual
    }

    PCB *currentProcess = pm->currentProcess;


    // Guardar el RSP del proceso actual
    if (currentProcess && currentProcess->state == 2) { // RUNNING
        printString("[SCHED] Guardando proceso actual PID: ");
        printHex64(currentProcess->pid);
        newLine();
        currentProcess->stackPointer = (uint64_t *)current_rsp;
        currentProcess->state = 1; // READY
        enqueue(pm->readyQueue, currentProcess);
    }
    
    // Obtener el siguiente proceso
    PCB *nextProcess = dequeue(pm->readyQueue);
    if (!nextProcess) {
        printString("[SCHED] No hay procesos en la cola");
        newLine();
        // No hay procesos listos, continuar con el actual
        if (currentProcess && currentProcess->state == 1) {
            currentProcess->state = 2;
            return current_rsp;
        }
        return current_rsp;
    }


    nextProcess->state = 2; // RUNNING
    pm->currentProcess = nextProcess;
    pm->currentPid = nextProcess->pid;

    return (uint64_t)nextProcess->stackPointer;
}