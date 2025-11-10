// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include "../include/userlib.h"
#include "../include/syscall.h"

extern void printTime(void);
extern void my_exit(void);

void time(uint64_t argc, char **argv){
    printTime();
    my_exit();
}