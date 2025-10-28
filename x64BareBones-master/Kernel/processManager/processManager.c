#include "processManager.h"
#include "list.h"
#include "priorityQueue.h"
#include "../include/time.h"
#include "../include/lib.h"
#include <string.h>

static void printDec64(uint64_t value)
{
    char buf[21]; // 20 dígitos + null terminator
    int i = 20;
    buf[i] = '\0';
    if (value == 0)
    {
        buf[--i] = '0';
    }
    else
    {
        while (value > 0 && i > 0)
        {
            buf[--i] = '0' + (value % 10);
            value /= 10;
        }
    }
    printString(&buf[i]);
}

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
    pm->currentPid = 0;
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

pid_t createProcess(ProcessManagerADT pm, void (*entryPoint)(int, char**), int priority, char *name, int argc, char **argv){
    pm->maxPid += 1;
    PCB *newProcess = (PCB *) allocateMemory(pm->memoryManager, sizeof(PCB));
    
    newProcess->pid = pm->maxPid;
    newProcess->parentPid = (pm->currentProcess != NULL) ? pm->currentProcess->pid : 0;
    newProcess->priority = priority;
    newProcess->state = 1; // Ready state
    newProcess->foreground = 1; // Foreground process
    newProcess->name = name;
    newProcess->stackSize = PROCESS_STACK_SIZE;
    newProcess->stackBase = (uint64_t *) allocateMemory(pm->memoryManager, newProcess->stackSize);
    newProcess->waitingToRead = 0;
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
    return newProcess->pid;
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

void printProcesses(ProcessManagerADT processManager) {
    printString("All Processes:");
    newLine();
    for (int i = 0; i < listSize(processManager->allProcesses); i++) {
        PCB *proc = getAt(processManager->allProcesses, i);
        printString("PID: ");
        printDec64(proc->pid);
        printString(", Name: ");
        printString(proc->name);
        printString(", State: ");
        printDec64(proc->state);
        printString(", Priority: ");
        printDec64(proc->priority);
        printString(", Stack Pointer: ");
        printHex64((uint64_t)(proc->stackPointer));
        printString(", Parent PID: ");
        printDec64(proc->parentPid);
        newLine();
    }
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
    if (!process) {
        printString("[DEBUG] unblock: Proceso PID ");
        printDec64(processId);
        printString(" no encontrado en lista de bloqueados");
        newLine();
        return;
    }

    if (process->state != 0) {
        printString("[DEBUG] unblock: Proceso PID ");
        printDec64(processId);
        printString(" no estaba bloqueado (estado: ");
        printDec64(process->state);
        printString(")");
        newLine();
        return;
    }

    printString("[DEBUG] unblock: Desbloqueando proceso PID ");
    printDec64(processId);
    newLine();

    process->state = 1;  // READY
    removeFromListByProcess(processManager->blockedProcesses, process);
    enqueue(processManager->readyQueue, process);

    printString("[DEBUG] unblock: Proceso PID ");
    printDec64(processId);
    printString(" ahora en READY");
    newLine();
}

void waitingToRead(ProcessManagerADT processManager, pid_t processId) {
    PCB *process = findInList(processManager->allProcesses, processId);  
    if (process) {
        process->waitingToRead = 1;
        block(processManager, processId);
    }
}

void unblockBecauseItRead(ProcessManagerADT processManager) {
    PCB * process = findFirstWaitingToRead(processManager->blockedProcesses);
    if (process) {
        process->waitingToRead = 0;
        unblock(processManager, process->pid);
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
    if (!child) {
        printString("[DEBUG] waitPid: Hijo PID ");
        printDec64(childPid);
        printString(" no encontrado");
        newLine();
        return;
    }

    printString("[DEBUG] waitPid: Padre PID ");
    printDec64(processManager->currentPid);
    printString(" esperando hijo PID ");
    printDec64(childPid);
    printString(" (estado hijo: ");
    printDec64(child->state);
    printString(")");
    newLine();

    if (child->state != 3) {  
        // Hijo no terminó, bloqueo al padre
        block(processManager, processManager->currentPid);
    } else {
        // el hijo termino, limpiamos
        printString("[DEBUG] waitPid: Hijo ya terminado, limpiando...");
        newLine();
        removeFromListByProcess(processManager->allProcesses, childPid);
        freeMemory(processManager->memoryManager, child->stackBase);
        freeMemory(processManager->memoryManager, child);
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

    // Guardamos el RSP del proceso actual si se esta ejecutando (RUNNING)
    if (currentProcess && currentProcess->state == 2) { // RUNNING
        currentProcess->stackPointer = (uint64_t *)current_rsp;
        // Solo vuelve a encolar si no termino ni esta bloqueado
        if (currentProcess->state != 3 && currentProcess->state != 0) {
            currentProcess->state = 1; // READY
            enqueue(pm->readyQueue, currentProcess);
        } else {
            // El proceso termino o se bloqueo, no lo vuelvo a encolar
            pm->currentProcess = NULL;
            pm->currentPid = 0;
        }
    }
    
    // Obtener el siguiente proceso
    PCB *nextProcess = dequeue(pm->readyQueue);
    if (!nextProcess) {
        // No hay procesos listos
        pm->currentProcess = NULL;
        pm->currentPid = 0;
        return current_rsp;
    }

    nextProcess->state = 2; // RUNNING
    pm->currentProcess = nextProcess;
    pm->currentPid = nextProcess->pid;

    return (uint64_t)nextProcess->stackPointer;
}

void exitProcess(ProcessManagerADT pm, pid_t processId) {
    PCB *process = findInList(pm->allProcesses, processId);  
    if (process) {
        printString("[DEBUG] exitProcess: Proceso PID ");
        printDec64(processId);
        printString(" terminando. Parent PID: ");
        printDec64(process->parentPid);
        newLine();

        process->state = 3;  // TERMINATED
        removeFromQueue(pm->readyQueue, process);

        // Desbloqueo al padre
        if (process->parentPid > 0) {
            PCB *parent = findInList(pm->allProcesses, process->parentPid);
            if (parent && parent->state == 0) {
                printString("[DEBUG] exitProcess: Desbloqueando padre PID ");
                printDec64(process->parentPid);
                newLine();
                unblock(pm, process->parentPid);
            }
        }

        // Limpiamos recursos del proceso terminado
        removeFromListByProcess(pm->allProcesses, processId);
        freeMemory(pm->memoryManager, process->stackBase);
        freeMemory(pm->memoryManager, process);

        printString("[DEBUG] exitProcess: Recursos liberados. Forzando context switch...");
        newLine();
    }
    printProcesses(pm);
    schedules(); // Forzar cambio de contexto
}