// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include "../include/userlib.h"
#include "../include/syscall.h"


void echo(uint64_t argc, char **argv) {
    write(getWriteFd(getPid()), argv[0], strlen(argv[0]));
    my_exit();
}