#include "../include/userlib.h"
#include "../include/syscall.h"

void getMyPid(uint64_t argc, char **argv){
    uint64_t pid = getPid();
    printf("PID del proceso actual: %d\n", pid);
    my_exit();
}