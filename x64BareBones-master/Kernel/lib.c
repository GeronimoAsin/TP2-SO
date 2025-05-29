#include <stdint.h>
#include <stdarg.h>
#include <stdint.h>
#include "syscallDispatcher.h"
#include "lib.h"
void * memset(void * destination, int32_t c, uint64_t length)
{
	uint8_t chr = (uint8_t)c;
	char * dst = (char*)destination;

	while(length--)
		dst[length] = chr;

	return destination;
}

void * memcpy(void * destination, const void * source, uint64_t length)
{
	/*
	* memcpy does not support overlapping buffers, so always do it
	* forwards. (Don't change this without adjusting memmove.)
	*
	* For speedy copying, optimize the common case where both pointers
	* and the length are word-aligned, and copy word-at-a-time instead
	* of byte-at-a-time. Otherwise, copy by bytes.
	*
	* The alignment logic below should be portable. We rely on
	* the compiler to be reasonably intelligent about optimizing
	* the divides and modulos out. Fortunately, it is.
	*/
	uint64_t i;

	if ((uint64_t)destination % sizeof(uint32_t) == 0 &&
		(uint64_t)source % sizeof(uint32_t) == 0 &&
		length % sizeof(uint32_t) == 0)
	{
		uint32_t *d = (uint32_t *) destination;
		const uint32_t *s = (const uint32_t *)source;

		for (i = 0; i < length / sizeof(uint32_t); i++)
			d[i] = s[i];
	}
	else
	{
		uint8_t * d = (uint8_t*)destination;
		const uint8_t * s = (const uint8_t*)source;

		for (i = 0; i < length; i++)
			d[i] = s[i];
	}

	return destination;
}


//scanf,printf,putChar,getChar


void putChar(char character)
{
	    char buffer[2] = {character, '\0'};

    sys_write(1,buffer); 
	return;  
}

char getChar()
{
	char buffer;
	sys_read(0,&buffer); 
	return buffer; 
}

void printf(const char *format, ...) {
    va_list args;
    va_start(args, format);

    while (*format) {
        if (*format == '%') {
            format++;
            switch (*format) {
                case 'c': { // Caracter
                    char c = (char)va_arg(args, int); // Los caracteres se pasan como int en argumentos variables
                    char buffer[2] = {c, '\0'};
                    sys_write(1, buffer);
                    break;
                }
                case 's': { // Cadena
                    const char *str = va_arg(args, const char *);
                    while (*str) {
                        char buffer[2] = {*str++, '\0'};
                        sys_write(1, buffer);
                    }
                    break;
                }
                case 'd': { // Entero
                    int num = va_arg(args, int);
                    char num_buffer[12]; // Buffer para almacenar el número como cadena
                    int num_len = 0;

                    if (num < 0) {
                        char buffer[2] = {'-', '\0'};
                        sys_write(1, buffer);
                        num = -num;
                    }

                    do {
                        num_buffer[num_len++] = (num % 10) + '0';
                        num /= 10;
                    } while (num > 0);

                    while (num_len > 0) {
                        char buffer[2] = {num_buffer[--num_len], '\0'};
                        sys_write(1, buffer);
                    }
                    break;
                }
                case '%': { // Literal '%'
                    char buffer[2] = {'%', '\0'};
                    sys_write(1, buffer);
                    break;
                }
                default:
                    // Si el formato no es reconocido, lo ignoramos
                    break;
            }
        } else {
            char buffer[2] = {*format, '\0'};
            sys_write(1, buffer);
        }
        format++;
    }

    va_end(args);
}

//caso de uso scanf("%d %s", &num, str);
//ingreso "42 hola"
//retorna 2 y asigna num=42 str="hola"
int scanf(const char *format, ...) {
    va_list args;
    va_start(args, format);
    int count = 0;

    while (*format) {
        if (*format == '%') {
            format++;
            switch (*format) {
                case 'd': { // Integer
                    int *iptr = va_arg(args, int*);
                    int num = 0, sign = 1, read = 0;
                    char c = getChar();
                    while (c == ' ' || c == '\n') c = getChar();
                    if (c == '-') { sign = -1; c = getChar(); }
                    while (c >= '0' && c <= '9') {
                        num = num * 10 + (c - '0');
                        c = getChar();
                        read = 1;
                    }
                    if (read) { *iptr = num * sign; count++; }
                    break;
                }
                case 'c': { // Char
                    char *cptr = va_arg(args, char*);
                    *cptr = getChar();
                    count++;
                    break;
                }
                case 's': { // String
                    char *sptr = va_arg(args, char*);
                    char c = getChar();
                    while (c == ' ' || c == '\n') c = getChar();
                    while (c != ' ' && c != '\n' && c != '\0') {
                        *sptr++ = c;
                        c = getChar();
                    }
                    *sptr = '\0';
                    count++;
                    break;
                }
                default:
                    break;
            }
        }else if (*format == '\n') {
            char buffer[2] = {'\n', '\0'};
            sys_write(1, buffer);
            char cr[2] = {'\r', '\0'};
            sys_write(1, cr);
        } else {
            char buffer[2] = {*format, '\0'};
            sys_write(1, buffer);
        }
        format++;
    }
    va_end(args);
    return count;
}



