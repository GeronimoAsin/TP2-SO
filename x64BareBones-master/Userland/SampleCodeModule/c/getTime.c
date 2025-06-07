#include <stdint.h>
#include "userlib.h"
#include "getTime.h"
extern void syscall(uint64_t rax, uint64_t rbx, uint64_t rdx, uint64_t rcx);

void getTime(Time *t) {
    // syscall 3 = getTime
    syscall(3, 0, (uint64_t)t, 0); //syscall para obtener el tiempo
}

int bcdToDec(uint8_t bcd) {
    return ((bcd >> 4) * 10) + (bcd & 0x0F);
}

void printTime() {
    Time t;
    getTime(&t); //le paso el struct del tiempo a getTime

     // Hora local Argentina (UTC-3)
    int hourUTC = bcdToDec(t.hours);
    int hourARG = (hourUTC - 3 + 24) % 24;
    printf("UTC TIME: %d:%d:%d\n", hourUTC, bcdToDec(t.minutes), bcdToDec(t.seconds));
    printf("ARG TIME: %d:%d:%d\n", hourARG, bcdToDec(t.minutes), bcdToDec(t.seconds));  //imprimo la hora local de argentina

}