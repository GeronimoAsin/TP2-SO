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
    printHex64(pm);
    pm->maxPid = 0;
    pm->currentPid = 0;
    pm->readyQueue = createPriorityQueue(memoryManager);
    pm->allProcesses = createList(memoryManager);
    pm->blockedProcesses = createList(memoryManager);
    pm->memoryManager = memoryManager;
    pm->currentProcess = NULL;
    pm->idleProcess = NULL;
    globalProcessManager = pm;
    
    // Crear el proceso idle (PID 0, prioridad 0 - la más baja)
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
    
    // Preparar el stack del proceso idle
    uint64_t idle_stack_top = (uint64_t)(idleProc->stackBase) + PROCESS_STACK_SIZE;
    idleProc->stackPointer = (uint64_t *)fill_stack(idle_stack_top, (uint64_t)idle, 0, 0);
    
    pm->idleProcess = idleProc;
    
    // Agregar idle a la lista de todos los procesos pero NO a la ready queue
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
    newProcess->state = 1; // Ready state
    newProcess->foreground = foreground; // Foreground (1) o Background (0)
    newProcess->name = name;
    newProcess->stackSize = PROCESS_STACK_SIZE;
    newProcess->stackBase = (uint64_t *) allocateMemory(pm->memoryManager, newProcess->stackSize);
    newProcess->read_fd = 0;  // Default read fd
    newProcess->write_fd = 1; // Default write fd
    // Calcular el tope del stack (crece hacia abajo)
    uint64_t stack_top = (uint64_t)(newProcess->stackBase) + PROCESS_STACK_SIZE;

    newProcess->argc = argc;
    newProcess->argv = argv;
    newProcess->instructionPointer = (uint64_t *) entryPoint;

    // Usar fill_stack para inicializar el stack del proceso
    uint64_t new_rsp = fill_stack(stack_top, (uint64_t)entryPoint, (uint64_t)argc, (uint64_t *)argv);

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

pid_t createProcessWithFds(ProcessManagerADT pm, void (*entryPoint)(int, char**), int priority, char *name, int argc, char **argv, int foreground, int initial_read_fd, int initial_write_fd){
    pm->maxPid += 1;
    PCB *newProcess = (PCB *) allocateMemory(pm->memoryManager, sizeof(PCB));

    newProcess->pid = pm->maxPid;
    newProcess->parentPid = (pm->currentProcess != NULL) ? pm->currentProcess->pid : 0;
    newProcess->priority = priority;
    newProcess->state = 1; // Ready state
    newProcess->foreground = foreground; // Foreground (1) o Background (0)
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
    return -1; // Indicar que no se encontró el proceso
}

int getReadFd(ProcessManagerADT processManager, pid_t processId) {
    PCB *process = findInList(processManager->allProcesses, processId);  
    if (process) {
        return process->read_fd;
    }
    return -1; // Indicar que no se encontró el proceso
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

typedef struct ProcessInfo {
    pid_t pid;
    char *name;
    size_t state;
    size_t priority;
    uint64_t *stackPointer;
    pid_t parentPid;
    size_t foreground;
} ProcessInfo;


ProcessInfo * getProcesses(ProcessManagerADT processManager, size_t *outCount) {
    size_t count = listSize(processManager->allProcesses);
    *outCount = count;  // Guardar el count en el puntero de salida
    
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
        process->state = 3;  // TERMINATED
        removeFromQueue(processManager->readyQueue, process);
        
        // Si el padre está esperando (bloqueado), desbloquearlo
        if (process->parentPid > 0) {
            PCB *parent = findInList(processManager->allProcesses, process->parentPid);
            if (parent && parent->state == 0) {  // BLOCKED
                unblock(processManager, process->parentPid);
            }
        }
        
        // El proceso queda en estado TERMINATED en allProcesses
        // El padre será responsable de limpiarlo con waitPid
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
    if (process == NULL) {
        return;
    }

    if (process->state == 0 || process->state == 3) { // Already blocked or terminated
        return;
    }

    int oldState = process->state;  // Guardar estado anterior
    process->state = 0;  // BLOCKED

    if (oldState == 1) {  // Si estaba READY, remover de la ready queue
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

    process->state = 1;  // READY
    removeFromListByProcess(processManager->blockedProcesses, process);
    enqueue(processManager->readyQueue, process);
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
        return;
    }

    // Loop hasta que el hijo termine
    while (child->state != 3) {  
        block(processManager, processManager->currentPid);
        
        // Cuando se desbloquea, verificar de nuevo el estado del hijo
        child = findInList(processManager->allProcesses, childPid);
        if (!child) {
            return;
        }
    }
    
    // El hijo terminó, limpiamos recursos
    removeFromListByProcess(processManager->allProcesses, child);
    freeMemory(processManager->memoryManager, child->stackBase);
    freeMemory(processManager->memoryManager, child);
}

// Trae el primer proceso en background a foreground
// Retorna el PID del proceso, o -1 si no hay procesos en background
pid_t fg(ProcessManagerADT processManager) {
    // Buscar el primer proceso en background (foreground == 0) que esté en estado READY o RUNNING
    for (int i = 0; i < listSize(processManager->allProcesses); i++) {
        PCB *proc = getAt(processManager->allProcesses, i);
        if (proc->foreground == 0 && (proc->state == 1 || proc->state == 2)) {
            // Marcar como foreground
            proc->foreground = 1;
            
            // Si el proceso actual (shell) está en foreground, esperar a que termine
            if (processManager->currentProcess) {
                waitPid(processManager, proc->pid);
            }
            
            return proc->pid;
        }
    }
    
    return -1; // No hay procesos en background
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

    // Guardamos el RSP del proceso actual
    if (currentProcess) {
        currentProcess->stackPointer = (uint64_t *)current_rsp;
        
        // Si está en estado RUNNING, lo volvemos a READY y encolamos
        if (currentProcess->state == 2) { // RUNNING
            currentProcess->state = 1; // READY
            enqueue(pm->readyQueue, currentProcess);
        } 
        // Si está BLOCKED (0) o TERMINATED (3), no lo encolamos
        // El stackPointer ya fue guardado para cuando se desbloquee o se limpie
    }
    
    // Obtener el siguiente proceso
    PCB *nextProcess = dequeue(pm->readyQueue);
    if (!nextProcess) {
        // No hay procesos listos, usar el proceso idle
        nextProcess = pm->idleProcess;
    }

    nextProcess->state = 2; // RUNNING
    pm->currentProcess = nextProcess;
    pm->currentPid = nextProcess->pid;

    return (uint64_t)nextProcess->stackPointer;
}

void exitProcess(ProcessManagerADT pm, pid_t processId) {
    PCB *process = findInList(pm->allProcesses, processId);  
    if (process) {
        process->state = 3;  // TERMINATED
        removeFromQueue(pm->readyQueue, process);
        
        // Si el padre está esperando (bloqueado), desbloquearlo
        if (process->parentPid > 0) {
            PCB *parent = findInList(pm->allProcesses, process->parentPid);
            if (parent && parent->state == 0) {  // BLOCKED (esperando en waitPid)
                unblock(pm, process->parentPid);
            }
        }
        
        // El proceso queda en estado TERMINATED en allProcesses
        // El padre será responsable de limpiarlo con waitPid
        // (Ya sea proceso foreground o background)
    }
    schedules(); // Forzar cambio de contexto
}