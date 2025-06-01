#include <stdint.h>
#include "userlib.h"
extern void syscall(uint64_t rax, uint64_t rbx, uint64_t rdx, uint64_t rcx);

uint64_t getTime() {
    // syscall 3 = getTime
    uint64_t time;
    syscall(3, 0, (uint64_t)&time, 0);
    return time;
}

void printTime() {
    uint64_t time = getTime();
    uint64_t seconds = time % 60;
    uint64_t minutes = (time / 60) % 60;
    uint64_t hours = time / 3600;
   printf("%d:%d:%d\n", hours, minutes, seconds);
}