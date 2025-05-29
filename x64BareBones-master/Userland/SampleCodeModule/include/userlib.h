#ifndef USERLIB_H
#define USERLIB_H
void putChar(char character);
char getChar();
void printf(const char *format, ...);
int scanf(const char *format, ...);
void signed_numtostr(int num, char *str);
void strToMinus(char *str);
void strToMayus(char *str);
void unsigned_numtohex(unsigned int num, char *str);
void signed_numtostr(int num, char *str);
int strcmp(const char *s1, const char *s2);
#endif //USERLIB_H
