/* sampleCodeModule.c */
#include "../include/userlib.h"
#include "../include/shell.h"
char * v = (char*)0xB8000 + 79 * 2;



int main() {
	//entrada a la shell
	startShell();

	return 0; 

}