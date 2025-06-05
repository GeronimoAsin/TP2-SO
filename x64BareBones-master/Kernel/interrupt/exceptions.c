
#define ZERO_EXCEPTION_ID 0
#define INVALID_OPCODE_EXCEPTION_ID 6

static void zero_division();
static void invalid_opcode();


void exceptionDispatcher(int exception) {
	if (exception == ZERO_EXCEPTION_ID)
		zero_division();
	if(exception == INVALID_OPCODE_EXCEPTION_ID) {
        invalid_opcode();
    }
}

static void zero_division() {
	printString("ERROR: no se puede dividir por cero\n Ejecute registers para ver el estado de los registros\n");
	getRegisters();
}

static void invalid_opcode() {
    printString("ERROR: se ha intentado ejecutar una instrucción inválida\n Ejecute registers para ver el estado de los registros\n");
	getRegisters();
}