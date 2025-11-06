#include <stdint.h>
#include <stddef.h>
typedef int pid_t ;


void *malloc(size_t size);
void free(void *ptr);
uint64_t getPid(void);
pid_t createProcess(void (*start_routine)(int, char**), char *name, int argc, char **argv, int foreground);
void waitPid(pid_t pid);
pid_t fg(void);
void my_exit(void);
int64_t my_kill(pid_t pid);
int64_t my_block(uint64_t pid);
int64_t my_unblock(uint64_t pid);
void printProcesses(void);
void meminfo(void);
int getWriteFd(pid_t pid);
int getReadFd(pid_t pid);

void user_echo(const char *input);
void user_help(char *help_text);
void user_clear(void);
void user_time(void);
void user_registers(void);
void user_memtest(void);
void user_memchunks(void);
void user_meminfo(void);

void foo(void);
uint64_t my_sem_open(char *sem_id, uint64_t initialValue) ;
uint64_t my_sem_wait(char *sem_id) ;
uint64_t my_sem_post(char *sem_id) ;
uint64_t my_sem_close(char *sem_id) ;

int strlen(const char *str);
int strcmp(const char *s1, const char *s2);
int strncmp(const char *s1, const char *s2, unsigned int n);
void printf(const char *format, ...);
void putChar(char c);
char getChar(void);
int read(int fd, char *buffer, int count);
int write(int fd, const char *buffer, int count);
int clearScreen(void);
int drawCursor(void);
int clearCursor(void);
int beep(void);
int deleteLastChar(void);
void printTime(void);
void print_registers(void);
void printCharWithColor(char c, uint32_t color);
