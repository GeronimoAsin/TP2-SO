#include "userlib.h"
#include "../include/memoryInfo.h"

typedef struct ProcessInfo {
    pid_t pid;
    char *name;
    size_t state;
    size_t priority;
    uint64_t *stackPointer;
    pid_t parentPid;
    size_t foreground;
} ProcessInfo;

int read(int fd, char *buffer, int count);
int write(int fd, const char *buffer, int count);
int clearScreen(void);
int drawCursor(void);
int clearCursor(void);
int deleteLastChar(void);
uint64_t getRegisters(uint64_t *regs) ;
void *malloc(size_t size);
void free(void *ptr);
uint64_t getPid(void);
pid_t createProcess(void (*start_routine)(int, char**), char *name, int argc, char **argv, int foreground);
void printProcesses();
pid_t fg(void);
int64_t my_kill(pid_t pid);
int64_t my_nice(uint64_t pid, uint64_t newPrio);
int64_t my_block(uint64_t pid);
int64_t my_unblock(uint64_t pid);
int64_t my_yield();
void meminfo(void);
void waitPid(pid_t pid);
void my_exit(void);
uint64_t my_sem_open(char *sem_id, uint64_t initialValue) ;
uint64_t my_sem_wait(char *sem_id) ;
uint64_t my_sem_post(char *sem_id) ;
uint64_t my_sem_close(char *sem_id) ;
int getWriteFd(pid_t pid);
int getReadFd(pid_t pid);
void setWriteFd(pid_t pid, int write_fd) ;
void setReadFd(pid_t pid, int read_fd) ;
void pipe(int fd[2]) ;
void closePipe(int fd[2]) ;
void printCharWithColor(char c, uint32_t color);
pid_t createProcessWithFds(void (*start_routine)(int, char**), char *name, int argc, char **argv, int foreground, int initial_read_fd, int initial_write_fd);
pid_t createProcessWithFdsSys(void (*start_routine)(int, char**), char *name, int argc, char **argv, int foreground, int read_fd, int write_fd);

// Wrappers para compatibilidad con tests
int64_t my_create_process(char *name, uint64_t argc, char *argv[]);
int64_t my_wait(int64_t pid);
int64_t my_getpid();