#include "userlib.h"
#include <stdint.h>
#include <stdarg.h>

extern uint64_t syscall(uint64_t rax, uint64_t rbx, uint64_t rdx, uint64_t rcx);


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
      write(1, buffer, 1);
}

char getChar() {
    char c;
    read(0, &c, 1);
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
            if (*format == 'l' && *(format+1) == 'l' && *(format+2) == 'x') {
                format += 2;
                uint64_t num = va_arg(args, uint64_t);
                char hex_buffer[17];
                unsigned_numtohex64(num, hex_buffer);
                const char *str = hex_buffer;
                while (*str) {
                    char buffer[2] = {*str++, '\0'};
                    write(1, buffer, 1);
                }
            } else {
                switch (*format) {
                    case 'c': { // Caracter
                        char c = (char)va_arg(args, int); // Los caracteres se pasan como int en argumentos variables
                        char buffer[2] = {c, '\0'};
                        write(1, buffer, 1);
                        break;
                    }
                    case 's': { // Cadena
                        const char *str = va_arg(args, const char *);
                        while (*str) {
                            char buffer[2] = {*str++, '\0'};
                            write(1, buffer, 1);
                        }
                        break;
                    }
                    case 'd': { // Entero
                        int num = va_arg(args, int);
                        char num_buffer[12]; // Buffer para almacenar el número como cadena
                        int num_len = 0;
                        if (num < 0) {
                            char buffer[2] = {'-', '\0'};
                            write(1, buffer, 1);
                            num = -num;
                        }
                        do {
                            num_buffer[num_len++] = (num % 10) + '0';
                            num /= 10;
                        } while (num > 0);
                        while (num_len > 0) {
                            char buffer[2] = {num_buffer[--num_len], '\0'};
                            write(1, buffer, 1);
                        }
                        break;
                    }
                    case 'x': { // Hexadecimal (unsigned int)
                        unsigned int num = va_arg(args, unsigned int);
                        char hex_buffer[9];
                        unsigned_numtohex(num, hex_buffer);
                        const char *str = hex_buffer;
                        while (*str) {
                            char buffer[2] = {*str++, '\0'};
                            write(1, buffer, 1);
                        }
                        break;
                    }
                    case '%': { // Literal '%'
                        char buffer[2] = {'%', '\0'};
                        write(1, buffer, 1);
                        break;
                    }
                    default:
                        break;
                }
            }
        } else {
            char buffer[2] = {*format, '\0'};
            write(1, buffer, 1);
        }
        format++;
    }

    va_end(args);
}


// Convierte un uint64_t a string hexadecimal (en minúsculas)
void unsigned_numtohex64(uint64_t num, char *str) {
    const char hexDigits[] = "0123456789abcdef";
    int i = 0;
    if (num == 0) {
        str[i++] = '0';
        str[i] = '\0';
        return;
    }
    while (num > 0) {
        str[i++] = hexDigits[num & 0xF]; // Uso AND en vez de % para evitar división
        num >>= 4;
    }
    str[i] = '\0';
    // Invierto el string
    for (int j = 0, k = i - 1; j < k; j++, k--) {
        char temp = str[j];
        str[j] = str[k];
        str[k] = temp;
    }
}


//Syscalls:

int read(int fd, char *buffer, int count) {
    return syscall(0, fd, (uint64_t)buffer, count);
}

int write(int fd, const char *buffer, int count) {
    return syscall(1, fd, (uint64_t)buffer, count);
}

int clearScreen() {
    return syscall(2, 0, 0, 0);
}

int drawCursor() {
    return syscall(7, 0, 0, 0);
}

int clearCursor() {
    return syscall(8, 0, 0, 0);
}

int beep() {
    return syscall(6, 0, 0, 0);
}

int deleteLastChar() {
    return syscall(5, 0, 0, 0);
}

uint64_t getRegisters(uint64_t *regs) {
    return syscall(4, (uint64_t)regs, 0, 0);
}

int getTime(uint64_t *t) {
    return syscall(3, 0, (uint64_t)t, 0);
}

void * malloc(size_t size)
{
    return (void *)syscall(10, (uint64_t)size, 0, 0);
}

void free(void *ptr)
{
    syscall(11, (uint64_t)ptr, 0, 0);
}