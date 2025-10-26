#include <time.h>
#include <stdint.h>
#include "../processManager/processManager.h"

static unsigned long ticks = 0;

void timer_handler(uint64_t *savedContext) {
	ticks++;
	ProcessManagerADT pm = getGlobalProcessManager();
	if (pm){
		scheduler_tick(pm, savedContext);
	}
}

int ticks_elapsed() {
	return ticks;
}

int seconds_elapsed() {
	return ticks / 18;
}

void schedule(){
	ProcessManagerADT pm = getGlobalProcessManager();
	if (pm){
		scheduler_tick(pm, getGlobalSavedContext());
	}
}