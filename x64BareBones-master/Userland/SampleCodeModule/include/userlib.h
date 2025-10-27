#ifndef USERLIB_H
#define USERLIB_H
#include <stdint.h>
#include <stddef.h>
void putChar(char c);
char getChar();
//void printf(const char *format, ...);
//int scanf(const char *format, ...);
void signed_numtostr(int num, char *str);
void strToMinus(char *str);
void strToMayus(char *str);
void unsigned_numtohex(unsigned int num, char *str);
void signed_numtostr(int num, char *str);
int strcmp(const char *s1, const char *s2);
//int strncmp(const char *s1, const char *s2, unsigned int n);
void * malloc(size_t size);
void free(void *ptr);
void meminfo();
void user_echo(const char *input);
void user_help(void);
void user_clear(void);
void user_time(void);
void user_registers(void);
void user_memtest(void);
void user_memchunks(void);
void user_meminfo(void);
#endif //USERLIB_H
