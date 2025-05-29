#include "../include/time.h"
#include <stdint.h>
#include "../include/keyboardDriver.h"

static void int_20();
static void int_21();

void (* functions[2])()={int_20, int_21};

void irqDispatcher(uint64_t irq) {
	functions[irq]();
	return;
}

void int_20() {
	timer_handler();
}

void int_21(){
	readAndProcess();
}
