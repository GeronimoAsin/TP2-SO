// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include "../include/time.h"
#include <stdint.h>
#include "../include/keyboardDriver.h"
#include "../include/videoDriver.h"

static void int_20();
static void int_21();

void (* functions[2])(uint64_t *)={int_20, int_21};

void irqDispatcher(uint64_t irq, uint64_t *savedContext) {
	functions[irq](savedContext);
	return;
}

void int_20(uint64_t *savedContext) {
	timer_handler(savedContext);
}

void int_21(uint64_t *savedContext){
	readAndProcess();
}
