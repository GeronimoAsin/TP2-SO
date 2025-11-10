#include "../include/userlib.h"
#include "../include/syscall.h"

extern void printTime(void);
extern void my_exit(void);

void time(uint64_t argc, char **argv){
    printTime();
    my_exit();
}