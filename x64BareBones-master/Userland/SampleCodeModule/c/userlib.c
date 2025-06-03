#include "userlib.h"
#include <stdint.h>
#include <stdarg.h>
extern void syscall(uint64_t rax, uint64_t rbx, uint64_t rdx, uint64_t rcx);


void unsigned_numtostr(unsigned int num, char *str) {
    int i = 0;

    if (num == 0) {
        str[i++] = '0';
        str[i] = '\0';
        return;
    }

    while (num > 0) {
        str[i++] = (num % 10) + '0';
        num /= 10;
    }

    str[i] = '\0';

    for (int j = 0, k = i - 1; j < k; j++, k--) {
        char temp = str[j];
        str[j] = str[k];
        str[k] = temp;
    }
}

void signed_numtostr(int num, char *str) {
    if (num < 0) {
        *str++ = '-';
        num = -num;
    }
    unsigned_numtostr((unsigned int)num, str);
}

void strToMinus(char *str)
{
    for (int i = 0; str[i]; i++)
    {
        if (str[i] >= 'A' && str[i] <= 'Z')
            str[i] += ('a' - 'A');
    }
}

void unsigned_numtohex(unsigned int num, char *str) {
    const char hexDigits[] = "0123456789ABCDEF";
    int i = 0;

    if (num == 0) {
        str[i++] = '0';
        str[i] = '\0';
        return;
    }

    while (num > 0) {
        str[i++] = hexDigits[num % 16];
        num /= 16;
    }

    str[i] = '\0';

    for (int j = 0, k = i - 1; j < k; j++, k--) {
        char temp = str[j];
        str[j] = str[k];
        str[k] = temp;
    }
}

void strToMayus(char *str) {
    for (int i = 0; str[i]; i++) {
        if (str[i] >= 'a' && str[i] <= 'z') {
            str[i] -= ('a' - 'A');
        }
    }
}


int strcmp(const char *s1, const char *s2)
{
    int i, toReturn = 0, checked = 0;
    for (i = 0; s1[i] && s2[i]; i++)
    {
        if (!checked)
        {
            toReturn += (s1[i] - s2[i]);
            checked = toReturn == 0 ? 0 : 1;
        }
    }
    if (s1[i])
    {
        toReturn = s1[i];
    }
    else if (s2[i])
    {
        toReturn = s2[i];
    }
    return toReturn;
}


  void putChar(char c) {
    char buffer[2];
      buffer[0] = c;
      syscall(1, 1, (uint64_t)buffer, 1);
  }

  char getChar() {
    char c;
    syscall(0, 0, (uint64_t)&c, 1);
    return c;
  }


int strncmp(const char *s1, const char *s2, unsigned int n) {
    unsigned int i = 0;
    while (i < n && s1[i] && s2[i]) {
        if (s1[i] != s2[i])
            return (unsigned char)s1[i] - (unsigned char)s2[i];
        i++;
    }
    if (i == n)
        return 0;
    return (unsigned char)s1[i] - (unsigned char)s2[i];
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
                    syscall(1, 1, (uint64_t)buffer, 1);
                    break;
                }
                case 's': { // Cadena
                    const char *str = va_arg(args, const char *);
                    while (*str) {
                        char buffer[2] = {*str++, '\0'};
                        syscall(1, 1, (uint64_t)buffer, 1);
                    }
                    break;
                }
                case 'd': { // Entero
                    int num = va_arg(args, int);
                    char num_buffer[12]; // Buffer para almacenar el número como cadena
                    int num_len = 0;

                    if (num < 0) {
                        char buffer[2] = {'-', '\0'};
                        syscall(1, 1, (uint64_t)buffer, 1);
                        num = -num;
                    }

                    do {
                        num_buffer[num_len++] = (num % 10) + '0';
                        num /= 10;
                    } while (num > 0);

                    while (num_len > 0) {
                        char buffer[2] = {num_buffer[--num_len], '\0'};
                        syscall(1, 1, (uint64_t)buffer, 1);
                    }
                    break;
                }
                case '%': { // Literal '%'
                    char buffer[2] = {'%', '\0'};
                    syscall(1, 1, (uint64_t)buffer, 1);
                    break;
                }
                default:
                    // Si el formato no es reconocido, lo ignoramos
                    break;
            }
        } else {
            char buffer[2] = {*format, '\0'};
            syscall(1, 1, (uint64_t)buffer, 1);
        }
        format++;
    }

    va_end(args);
}

