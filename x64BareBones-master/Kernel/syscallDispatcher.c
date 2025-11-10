#include "syscallDispatcher.h"
#include "videoDriver.h"
#include "keyboardDriver.h"
#include <stdarg.h>
#include <string.h>
#include "memoryManager/memoryManager.h"
#include "processManager/processManager.h"
#include "semaphores/semaphores.h"
#include "pipes/pipeManager.h"
#define REGISTERS 18

typedef struct MemoryManagerCDT * MemoryManagerADT;
MemoryInfo *sys_meminfo(void);


//struct para obtener el tiempo
typedef struct {
    uint8_t hours;
    uint8_t minutes;
    uint8_t seconds;
} Time;

void sys_getTime(Time *t);
uint8_t getHours();
uint8_t getMinutes();
uint8_t getSeconds();

extern uint64_t savedRegisters[]; // Declaración del buffer ASM

uint64_t sys_getRegisters(uint64_t *dest);
extern uint64_t getTime();
extern uint64_t getRegisters();

static MemoryManagerADT kernelMemoryManager = NULL;

uint64_t syscallDispatcher(uint64_t id, ...)
{
    uint64_t rbx, rdx, rcx, r8, r9;
    va_list args;
    va_start(args, id);
    rbx = va_arg(args, uint64_t);
    rdx = va_arg(args, uint64_t);
    rcx = va_arg(args, uint64_t);
    r8 = va_arg(args, uint64_t);
    r9 = va_arg(args, uint64_t);
    va_end(args);

    //switch case para llamar a las syscalls segun id
    switch (id)
    {
        case 0:
            return sys_read(rbx, (char*) rdx, rcx);
        case 1:
            return sys_write(rbx, (const char *)rdx, rcx);
        case 2:
            clearScreen(0x00000000); 
            return 1;
        case 3:
            sys_getTime((Time *) rdx);
            return 1;
        case 4:
            return sys_getRegisters((uint64_t *)rbx);
        case 5:
            deleteLastChar();
            return 1;
        case 7:
          	drawCursor();
            return 1;
        case 8:
          	clearCursor();
            return 1;
        case 9: 
            setCursor((int)rbx, (int)rdx);
            return 1;
        case 10: // malloc
            {
                void *p = sys_malloc(rbx);
                return (uint64_t)p;
}
        case 11: // free
            sys_free((void *) rbx);
            return 1;
        case 12:
            return (uint64_t) sys_meminfo();
        case 13:
            // rbx=entryPoint, rdx=foreground, rcx=name, r8=argc, r9=argv
            return createProcess(getGlobalProcessManager(), (void (*) (int, char**)) rbx, 1, (char *) rcx, (int) r8, (char **) r9, (int) rdx);
        case 14:
            return getPid(getGlobalProcessManager());
        case 15:
            return (uint64_t) getProcesses(getGlobalProcessManager(), (size_t *) rbx);
        case 18:
            waitPid(getGlobalProcessManager(), (pid_t) rbx);
            return 1;
        case 19:
            exitProcess(getGlobalProcessManager(), (pid_t) rbx);
            return 1;
        case 20:
            return (uint64_t) fg(getGlobalProcessManager());
        case 21:
            kill(getGlobalProcessManager(), (pid_t) rbx);
            return 1;
        case 22: // my_nice: pid en rbx, nueva prioridad en rdx
            modifyPriority(getGlobalProcessManager(), (pid_t) rbx, (int) rdx);
            return 1;
        case 23: // my_block: pid en rbx
            block(getGlobalProcessManager(), (pid_t) rbx);
            return 1;
        case 24: // my_unblock: pid en rbx
            unblock(getGlobalProcessManager(), (pid_t) rbx);
            return 1;
        case 25: // my_yield
            leaveCPU(getGlobalProcessManager());
            return 1;
        case 26: //my_sem_open
            sem_open(getGlobalSemaphoresManager(), (char *) rbx, (int) rdx);
            return 1;
        case 27: //my_sem_wait
            sem_wait(getGlobalSemaphoresManager(), (char *) rbx);
            return 1;
        case 28: //my_sem_post
            sem_post(getGlobalSemaphoresManager(), (char *) rbx);
            return 1;
        case 29: //my_sem_close
            sem_close(getGlobalSemaphoresManager(), (char *) rbx);
            return 1;
        case 30: //setWriteFd
            setWriteFd(getGlobalProcessManager(), (pid_t) rbx, (int) rdx);
            return 1;
        case 31: //setReadFd
            setReadFd(getGlobalProcessManager(), (pid_t) rbx, (int) rdx);
            return 1;
        case 32: //pipeCreate
            addPipe(getGlobalPipeManager(), (int *) rbx);
            return 1;
        case 33://closePipe
            closePipe(getGlobalPipeManager(), (int *) rbx);
            return 1;
        case 34: //getWriteFd
            return (uint64_t)getWriteFd(getGlobalProcessManager(), (pid_t) rbx);
        case 35: //getReadFd
            return (uint64_t)getReadFd(getGlobalProcessManager(), (pid_t) rbx);
        case 36: //printCharWithColor: rbx=char, rdx=color
            printCharWithColor((char)rbx, (uint32_t)rdx);
            return 1;
        case 37: { // createProcessWithFds: rbx=entry, rdx=foreground, rcx=name, r8=ptr args
            typedef struct { int argc; char **argv; int read_fd; int write_fd; } ProcArgs;
            ProcArgs *pa = (ProcArgs *) r8;
            return createProcessWithFds(getGlobalProcessManager(), (void (*)(int, char**)) rbx, 1, (char *) rcx, pa->argc, (char **)pa->argv, (int) rdx, pa->read_fd, pa->write_fd);
        }
        default:
            return -1;
    }   
}


uint64_t sys_read(uint64_t fd, char *buff, uint64_t count)
{

    uint64_t i = 0;
    if(fd==0){
        char character;

        while (i < count && (character = nextFromBuffer()) != 0) {
            if(character == -1) { // EOF
                return 0;;
            }
            buff[i++] = character;
        }
    }else{
        PipeManagerADT pipeManager = getGlobalPipeManager();
        i = readFromPipe(pipeManager, fd, buff, count);
    }
    
    return i;
}


uint64_t sys_write(uint64_t fd, const char *buffer, uint64_t count)
{

    uint64_t i;
    if(fd==1){
        for (i = 0; i < count; i++) {
            if (buffer[i] == '\n') {
                newLine();
            } else if (buffer[i] == '\b') {
                deleteLastChar();
            } else {
                printChar(buffer[i]);
            }
        }
    }else{
        PipeManagerADT pipeManager = getGlobalPipeManager();
        i = writeToPipe(pipeManager, fd, buffer, count);
    }

    return i; 

}

void sys_getTime(Time *t)
{
    t->hours = getHours(); //obtengo las horas
    t->minutes = getMinutes(); //obtengo los minutos
    t->seconds = getSeconds(); //obtengo los segundos
}


uint64_t sys_getRegisters(uint64_t *dest) {
    if (dest == 0){
        return -1;
    }

    memcpy(dest, savedRegisters, REGISTERS * sizeof(uint64_t));
    return 1;
}


void * sys_malloc(size_t size)
{
    if (size == 0) return NULL;

    if (kernelMemoryManager == NULL) {
        kernelMemoryManager = createMemoryManager();
    }

    return allocateMemory(kernelMemoryManager, size);
}

void sys_free(void *ptr)
{
    if (ptr == NULL) return;
    if (kernelMemoryManager == NULL) return; 
    freeMemory(kernelMemoryManager, ptr);
}

MemoryInfo *sys_meminfo()
{
    return meminfo();
}
