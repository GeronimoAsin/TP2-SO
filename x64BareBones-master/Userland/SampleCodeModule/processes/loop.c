#include "../c/userlib.h"

void foo() {
    printf("Proceso foo (PID %d) ejecutandose...\n", getPid());
    printProcesses();
    for(int i= 0; i<10000000000; i++);
    printf("END\n");
    exit(getPid());
}