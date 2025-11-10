// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
/* sampleCodeModule.c */
#include "../include/userlib.h"
#include "../include/shell.h"
char * var = (char*)0xB8000 + 79 * 2;



int main() {
	//entrada a la shell
	startShell();

	return 0; 

}