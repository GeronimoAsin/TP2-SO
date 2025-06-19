#include "stdint.h"
#include "videoDriver.h"
#define ZERO_EXCEPTION_ID 0
#define INVALID_OPCODE_EXCEPTION_ID 6

extern uint8_t getSeconds();
static void zero_division();
static void invalid_opcode();
void printHex(uint64_t value);

void waitNSeconds(uint8_t secondsToWait) {
	uint8_t start = getSeconds();
	uint8_t now;
	while (1) {
		now = getSeconds();
		int delta = now >= start ? (now - start) : (now + 60 - start);
		if (delta >= secondsToWait) {
			break;
		}
	}
}

void exceptionDispatcher(int exception, uint64_t rip, uint64_t rflags, uint64_t cs) {
	if (exception == ZERO_EXCEPTION_ID)
		zero_division();
	if(exception == INVALID_OPCODE_EXCEPTION_ID) {
        invalid_opcode();
    }
	newLine();
	printString("Valor de RIP: ");
	newLine();
	printHex(rip);
	printString("Valor de CS: ");
	newLine();
	printHex(cs);
	printString("Valor de RFLAGS: ");
	newLine();
	printHex(rflags);
	printString("Ejecute registers para conocer el estado de los demas registros.");
	newLine();
}

static void zero_division() {
	printString("ERROR: no se puede dividir por cero. ");

}

static void invalid_opcode() {
    printString("ERROR: se ha intentado ejecutar una instruccion invalida.");
}

void printHex(uint64_t value) {
	char hexChars[] = "0123456789ABCDEF";
	char buffer[17];
	int i;

	for (i = 15; i >= 0; i--) {
		buffer[i] = hexChars[value & 0xF];
		value >>= 4;
	}
	buffer[16] = '\0';

	printString("0x");
	for (i = 0; i < 16; i++) {
		printChar(buffer[i]);
	}
	newLine();
}