#include <time.h>
#include <stdint.h>

static unsigned long ticks = 0;

void timer_handler(uint64_t *savedContext) {
	ticks++;
}

int ticks_elapsed() {
	return ticks;
}

int seconds_elapsed() {
	return ticks / 18;
}

