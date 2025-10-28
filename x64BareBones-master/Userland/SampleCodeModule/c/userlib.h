#include <stdint.h>
#include <stddef.h>
typedef int pid_t ;

void *malloc(size_t size);
void free(void *ptr);
uint64_t getPid(void);
void createProcess(void (*start_routine)(int, char**), char *name, int argc, char **argv);
void printProcesses(void);
void meminfo(void);

void user_echo(const char *input);
void user_help(char *help_text);
void user_clear(void);
void user_time(void);
void user_registers(void);
void user_memtest(void);
void user_memchunks(void);
void user_meminfo(void);

void foo(void);
