// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include <stdint.h>
#include "userlib.h"
#include "getTime.h"
extern void syscall(uint64_t rax, uint64_t rbx, uint64_t rdx, uint64_t rcx);

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

void waitNSeconds(uint8_t secondsToWait) {
    Time start;
    getTime(&start);
    while (1) {
        Time now;
        getTime(&now);
        int delta = now.seconds >= start.seconds ? (now.seconds - start.seconds) : (now.seconds + 60 - start.seconds);
        if (delta >= secondsToWait) {
            break;
        }
    }
}