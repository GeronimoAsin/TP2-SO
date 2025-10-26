#ifndef _TIME_H_
#define _TIME_H_

#include <stdint.h>

void timer_handler(uint64_t *savedContext) ;
int ticks_elapsed();
int seconds_elapsed();
void schedule();

#endif
