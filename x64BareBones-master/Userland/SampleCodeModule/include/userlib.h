#include <stdint.h>
#include <stddef.h>
typedef int pid_t ;


void unsigned_numtostr(unsigned int num, char *str);
void signed_numtostr(int num, char *str) ;
void strToMinus(char *str);
void unsigned_numtohex(unsigned int num, char *str);
void strToMayus(char *str);
int strcmp(const char *s1, const char *s2);
int strlen(const char *str);
void putChar(char c);
char getChar();
int strncmp(const char *s1, const char *s2, unsigned int n);
void printf(const char *format, ...);
void printHex64(uint64_t value);
void unsigned_numtohex64(uint64_t num, char *str);
void user_memchunks(void);