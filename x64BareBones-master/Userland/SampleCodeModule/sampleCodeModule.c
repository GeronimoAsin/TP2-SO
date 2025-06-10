/* sampleCodeModule.c */
#include "../include/userlib.h"
#include "../include/shell.h"
char * v = (char*)0xB8000 + 79 * 2;

static int var1 = 0;
static int var2 = 0;


int main() {
	//entrada a la shell
	startShell();

	return 0; 

}