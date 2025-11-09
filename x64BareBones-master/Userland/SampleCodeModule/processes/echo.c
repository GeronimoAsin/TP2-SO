#include "../include/userlib.h"
#include "../include/syscall.h"


void echo(uint64_t argc, char **argv) {
    write(getWriteFd(getPid()), argv[0], strlen(argv[0]));
    my_exit();
}