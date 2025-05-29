#ifndef LIB_H
#define LIB_H
#include <stdint.h>

void* memset(void * destination, int32_t character, uint64_t length);
void* memcpy(void * destination, const void * source, uint64_t length);
char *cpuVendor(char *result);


void putChar(char character); 
char getChar();
void printf(const char *format, ...);
int scanf(const char *format, ...);
void signed_numtostr(int num, char *str);
void strToMinus(char *str);
void strToMayus(char *str);
void unsigned_numtohex(unsigned int num, char *str);
void signed_numtostr(int num, char *str);
int strcmp(char *s1, char *s2);
#endif