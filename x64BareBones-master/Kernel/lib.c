#include <stdint.h>
#include <stdarg.h>
#include <stdint.h>

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
	sys_read(0,buffer); 
	return buffer; 
}

// Implementación de printf
int printf(const char *format, ...) {
    va_list args;
    va_start(args, format);

    int chars_written = 0;

    for (const char *ptr = format; *ptr != '\0'; ptr++) {
        if (*ptr == '%' && *(ptr + 1) != '\0') {
            ptr++; // Avanzar al especificador de formato
            if (*ptr == 's') {
                // Formato %s (cadena)
                const char *str = va_arg(args, const char *);
                while (*str != '\0') {
                    sys_write(1, *str++);
                    chars_written++;
                }
            } else if (*ptr == 'd') {
                // Formato %d (entero)
                int num = va_arg(args, int);
                char num_buffer[20];
                int num_len = 0;

                // Convertir entero a cadena
                if (num < 0) {
                    sys_write(1, '-');
                    chars_written++;
                    num = -num;
                }
                do {
                    num_buffer[num_len++] = '0' + (num % 10);
                    num /= 10;
                } while (num > 0);

                // Imprimir número en orden inverso
                while (num_len > 0) {
                    sys_write(1, num_buffer[--num_len]);
                    chars_written++;
                }
            } else if (*ptr == 'c') {
                // Formato %c (carácter)
                char c = (char)va_arg(args, int);
                sys_write(1, c);
                chars_written++;
            } else {
                // Formato desconocido, imprimir tal cual
                sys_write(1, '%');
                sys_write(1, *ptr);
                chars_written += 2;
            }
        } else {
            // Carácter normal
            sys_write(1, *ptr);
            chars_written++;
        }
    }

    va_end(args);

    return chars_written; // Retornar el número de caracteres escritos
}

