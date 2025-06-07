#include "stdint.h"
#include "videoDriver.h"
#define ZERO_EXCEPTION_ID 0
#define INVALID_OPCODE_EXCEPTION_ID 6

static void zero_division();
static void invalid_opcode();
void printHex(uint64_t value);

void exceptionDispatcher(int exception, uint64_t rip) {
	if (exception == ZERO_EXCEPTION_ID)
		zero_division();
	if(exception == INVALID_OPCODE_EXCEPTION_ID) {
        invalid_opcode();
    }
	newLine();
	printString("Valor de RIP: ");
	newLine();
	printHex(rip);
	for(int i = 0; i<1000000000; i++);
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