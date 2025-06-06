#include "stdint.h"
//struct para obtener el tiempo
typedef struct {
    uint8_t hours;
    uint8_t minutes;
    uint8_t seconds;
} Time;

void getTime(Time *t);
void printTime();