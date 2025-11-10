#include "../include/syscall.h"
#include <stdint.h>
#include <stdarg.h>
#include <stddef.h>
#include "../include/memoryInfo.h"

#define CHUNK_SIZE 4096

extern uint64_t syscall(uint64_t rax, uint64_t rbx, uint64_t rdx, uint64_t rcx, uint64_t r8, uint64_t r9);
void endless_loop(int argc, char *argv[]);
void zero_to_max(int argc, char *argv[]);
void my_process_inc(int argc, char *argv[]);


int read(int fd, char *buffer, int count) {
    if (fd == 0) {
        my_sem_wait("waiting_to_read");
    }
    return syscall(0, fd, (uint64_t)buffer, count, 0, 0);
}

int write(int fd, const char *buffer, int count) {
    return syscall(1, fd, (uint64_t)buffer, count, 0, 0);
}

int clearScreen() {
    return syscall(2, 0, 0, 0, 0, 0);
}

int drawCursor() {
    return syscall(7, 0, 0, 0, 0, 0);
}

int clearCursor() {
    return syscall(8, 0, 0, 0, 0, 0);
}

int deleteLastChar() {
    return syscall(5, 0, 0, 0, 0, 0);
}

uint64_t getRegisters(uint64_t *regs) {
    return syscall(4, (uint64_t)regs, 0, 0, 0, 0);
}

int getTime(uint64_t *t) {
    return syscall(3, 0, (uint64_t)t, 0, 0, 0);
}

void * malloc(size_t size)
{
    return (void *)syscall(10, (uint64_t)size, 0, 0, 0, 0);
}

void free(void *ptr)
{
    syscall(11, (uint64_t)ptr, 0, 0, 0, 0);
}

uint64_t getPid() {
    return syscall(14, 0, 0, 0, 0, 0);
}

pid_t createProcess(void (*start_routine)(int, char**),char * name, int argc, char **argv, int foreground) {
    pid_t pid = syscall(13, (uint64_t)start_routine, foreground, (uint64_t)name, argc, (uint64_t)argv);
    return pid;
}

void printProcesses() {
    size_t count;
    ProcessInfo *processes = syscall(15, &count, 0, 0, 0, 0);
    for (size_t i = 0; i < count; i++) {
        char *stateStr;
        if(processes[i].state == 0) {
            stateStr = "BLOCKED";
        } else if(processes[i].state == 1) {
            stateStr = "READY";
        } else if(processes[i].state == 2) {
            stateStr = "RUNNING";
        } else if(processes[i].state == 3) {
            stateStr = "TERMINATED";
        }
        printf("PID: %d, Name: %s, State: %s, Priority: %d, Parent PID: %d, Foreground: %d\n",
               processes[i].pid,
               processes[i].name,
               stateStr,
               processes[i].priority,
               processes[i].parentPid,
               processes[i].foreground);
    }
    free(processes);
}

pid_t fg() {
    return (pid_t) syscall(20, 0, 0, 0, 0, 0);
}

int64_t my_kill(pid_t pid) {
    return syscall(21, (uint64_t)pid, 0, 0, 0, 0);
}

int64_t my_nice(uint64_t pid, uint64_t newPrio) {
    return syscall(22, pid, newPrio, 0, 0, 0);
}

int64_t my_block(uint64_t pid) {
    return syscall(23, pid, 0, 0, 0, 0);
}

int64_t my_unblock(uint64_t pid) {
    return syscall(24, pid, 0, 0, 0, 0);
}

int64_t my_yield() {
    return syscall(25, 0, 0, 0, 0, 0);
}


void meminfo(void) {
    MemoryInfo *kernelInfo = (MemoryInfo *)syscall(12, 0, 0, 0, 0, 0);
    if (kernelInfo == NULL) {
        printf("No se pudo obtener información de memoria.\n");
        return;
    }

    MemoryInfo info = *kernelInfo; 
    uint64_t usedChunks = 0;
    uint64_t freeChunks = 0;
    if (info.chunkSize != 0) {
        usedChunks = (info.usedBytes + info.chunkSize - 1) / info.chunkSize;
        if (info.chunkCount > usedChunks) {
            freeChunks = info.chunkCount - usedChunks;
        } else {
            freeChunks = (info.freeBytes + info.chunkSize - 1) / info.chunkSize;
        }
    }

    uint64_t usedPercent = info.heapSize ? (info.usedBytes * 100) / info.heapSize : 0;
    uint64_t freePercent = info.heapSize ? (info.freeBytes * 100) / info.heapSize : 0;

    printf("--- Estado de la memoria ---\n");
    printf("Direccion base del heap: 0x%llx\n", (unsigned long long)info.heapStart);
    printf("Dimension total del heap: %llu bytes\n", (unsigned long long)info.heapSize);
    printf("Dimension de chunk: %llu bytes\n", (unsigned long long)info.chunkSize);
    printf("Cantidad total de chunks: %llu\n", (unsigned long long)info.chunkCount);
    printf("Chunks usados: %llu\n", (unsigned long long)usedChunks);
    printf("Chunks libres: %llu\n", (unsigned long long)freeChunks);
    printf("Memoria usada: %llu bytes\n", (unsigned long long)info.usedBytes);
    printf("Memoria libre: %llu bytes\n", (unsigned long long)info.freeBytes);
    printf("Uso de memoria: %llu%% usados / %llu%% libres\n", (unsigned long long)usedPercent, (unsigned long long)freePercent);
    printf("Asignaciones totales: %llu\n", (unsigned long long)info.totalAllocations);
    printf("Liberaciones totales: %llu\n", (unsigned long long)info.totalFrees);
    printf("Asignaciones fallidas: %llu\n", (unsigned long long)info.failedAllocations);
	printf("----------------------------\n");
}

void waitPid(pid_t pid) {
    syscall(18, (uint64_t)pid, 0, 0, 0, 0);
}

void my_exit() {
    pid_t currentPid = getPid();
    syscall(19, (uint64_t)currentPid, 0, 0, 0, 0);
}

uint64_t my_sem_open(char *sem_id, uint64_t initialValue) {
  return syscall(26, (uint64_t)sem_id, initialValue, 0, 0, 0);
}

uint64_t my_sem_wait(char *sem_id) {
  return syscall(27, (uint64_t)sem_id, 0, 0, 0, 0);
}

uint64_t my_sem_post(char *sem_id) {
  return syscall(28, (uint64_t)sem_id, 0, 0, 0, 0);
}

uint64_t my_sem_close(char *sem_id) {
  return syscall(29, (uint64_t)sem_id, 0, 0, 0, 0);
}

void setWriteFd(pid_t pid, int write_fd) {
    syscall(30, (uint64_t)pid, (uint64_t)write_fd, 0, 0, 0);
}

void setReadFd(pid_t pid, int read_fd) {
    syscall(31, (uint64_t)pid, (uint64_t)read_fd, 0, 0, 0);
}

int getWriteFd(pid_t pid) {
    return (int)syscall(34, (uint64_t)pid, 0, 0, 0, 0);
}

int getReadFd(pid_t pid) {
    return (int)syscall(35, (uint64_t)pid, 0, 0, 0, 0);
}

void pipe(int fd[2]) {
    syscall(32, (uint64_t)fd, 0, 0, 0, 0);
}

void closePipe(int fd[2]) {
    syscall(33, (uint64_t)fd, 0, 0, 0, 0);
}

void printCharWithColor(char c, uint32_t color) {
    syscall(36, (uint64_t)c, (uint64_t)color, 0, 0, 0);
}

pid_t createProcessWithFds(void (*start_routine)(int, char**), char *name, int argc, char **argv, int foreground, int initial_read_fd, int initial_write_fd) {
    pid_t pid = createProcess(start_routine, name, argc, argv, foreground);
    if (initial_read_fd >= 0) {
        setReadFd(pid, initial_read_fd);
    }
    if (initial_write_fd >= 0) {
        setWriteFd(pid, initial_write_fd);
    }
    return pid;
}

pid_t createProcessWithFdsSys(void (*start_routine)(int, char**), char *name, int argc, char **argv, int foreground, int read_fd, int write_fd) {
    struct ProcArgs { int argc; char **argv; int read_fd; int write_fd; } pa;
    pa.argc = argc; pa.argv = argv; pa.read_fd = read_fd; pa.write_fd = write_fd;
    return (pid_t) syscall(37, (uint64_t)start_routine, foreground, (uint64_t)name, (uint64_t)&pa, 0);
}

// Wrappers para compatibilidad con tests
int64_t my_getpid() {
    return (int64_t)getPid();
}

int64_t my_wait(int64_t pid) {
    waitPid((pid_t)pid);
    return 0;
}

int64_t my_create_process(char *name, uint64_t argc, char *argv[]) {
    void (*func)(int, char**) = NULL;
    
    if (strcmp(name, "endless_loop") == 0) {
        func = endless_loop;
    } else if (strcmp(name, "zero_to_max") == 0) {
        func = zero_to_max;
    } else if (strcmp(name, "my_process_inc") == 0) {
        func = my_process_inc;
    }
    
    if (func == NULL) {
        return -1; 
    }
    
    return (int64_t)createProcess(func, name, (int)argc, argv, 0);
}


