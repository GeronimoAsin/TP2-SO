// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include "../include/userlib.h"
#include "../include/syscall.h"


void foreground(uint64_t arcg, char **argv){
    pid_t fgPid = fg();
    if (fgPid == -1) {
        printf("No hay procesos en background\n");
    } else {
        printf("Proceso %d traido a foreground\n", fgPid);
    }
    my_exit();
}