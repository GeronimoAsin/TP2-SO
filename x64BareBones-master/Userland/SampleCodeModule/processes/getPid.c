// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include "../include/userlib.h"
#include "../include/syscall.h"

void getMyPid(uint64_t argc, char **argv){
    uint64_t pid = getPid();
    printf("PID del proceso actual: %d\n", (int)pid);
    my_exit();
}