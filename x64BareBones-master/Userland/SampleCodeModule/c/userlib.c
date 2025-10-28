#include "userlib.h"
#include <stdint.h>
#include <stdarg.h>
#include <stddef.h>

#define CHUNK_SIZE 4096

typedef struct MemoryManagerCDT {
    uint8_t * heapStart;
    unsigned int heapSize;
    unsigned int chunkSize;
    unsigned int chunkCount;
    unsigned int nextFreeIndex;
    uint8_t * freeChunkStack[]; // flexible array, no se usa aquí
} MemoryManagerCDT;

typedef MemoryManagerCDT * MemoryManagerADT;

extern uint64_t syscall(uint64_t rax, uint64_t rbx, uint64_t rdx, uint64_t rcx, uint64_t r8, uint64_t r9);


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


static void printHex64(uint64_t value) {
    char buf[19]; // "0x" + 16 hex + '\0'
    buf[0] = '0';
    buf[1] = 'x';
    for (int i = 0; i < 16; i++) {
        uint8_t nibble = (value >> ((15 - i) * 4)) & 0xF;
        buf[2 + i] = (nibble < 10) ? ('0' + nibble) : ('A' + (nibble - 10));
    }
    buf[18] = '\0';
    printf(buf);
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
    return syscall(0, fd, (uint64_t)buffer, count, 0, 0);
}

int write(int fd, const char *buffer, int count) {
    return syscall(1, fd, (uint64_t)buffer, count, 0, 0);
}

int clearScreen() {
    return syscall(2, 0, 0, 0, 0, 0);
}

int drawCursor() {
    return syscall(7, 0, 0, 0, 0, 0);
}

int clearCursor() {
    return syscall(8, 0, 0, 0, 0, 0);
}

int beep() {
    return syscall(6, 0, 0, 0, 0, 0);
}

int deleteLastChar() {
    return syscall(5, 0, 0, 0, 0, 0);
}

uint64_t getRegisters(uint64_t *regs) {
    return syscall(4, (uint64_t)regs, 0, 0, 0, 0);
}

int getTime(uint64_t *t) {
    return syscall(3, 0, (uint64_t)t, 0, 0, 0);
}

void * malloc(size_t size)
{
    return (void *)syscall(10, (uint64_t)size, 0, 0, 0, 0);
}

void free(void *ptr)
{
    syscall(11, (uint64_t)ptr, 0, 0, 0, 0);
}

int64_t my_getpid() {
    return syscall(12, 0, 0, 0, 0, 0);
}

void createProcess(void (*start_routine)(int, char**),char * name, int argc, char **argv) {
    syscall(13, (uint64_t)start_routine, 1, name ,argc, (uint64_t)argv);
}

void meminfo() {
    MemoryManagerADT mem = (MemoryManagerADT) syscall(12, 0, 0, 0, 0, 0);

 if (!mem) {
       printf("No se pudo obtener información de memoria.\n");
        return;
    }

    unsigned int usados = mem->chunkCount - mem->nextFreeIndex;
    unsigned int libres = mem->nextFreeIndex;
    unsigned int usado_bytes = usados * mem->chunkSize;
    unsigned int libre_bytes = libres * mem->chunkSize;
    printf("--- Estado de la memoria ---\n");
    printf("Direccion base del heap: 0x%x\n", mem->heapStart);
    printf("Dimension total del heap: %d bytes\n", mem->heapSize);
    printf("Dimension de chunk: %d bytes\n", mem->chunkSize);
    printf("Cantidad total de chunks: %u\n", mem->chunkCount);
    printf("Chunks libres: %d\n", libres);
    printf("Chunks usados: %d\n", usados);
    printf("Memoria libre: %d bytes\n", libre_bytes);
    printf("Memoria usada: %d bytes\n", usado_bytes);
    printf("----------------------------\n");


}

void user_echo(const char *input) {
    // Salta espacios o tabs iniciales
    while (*input == ' ' || *input == '\t') input++;
    printf("%s", input);
}

void user_help(char * help_text) {
    printf("%s", help_text);
}

void user_clear(void) {
    clearScreen();
}

void user_time(void) {
    printTime();
}

void user_registers(void) {
    print_registers();
}

void user_memtest(void) {
    printf("=== Prueba de memoria ===\n");
    void *p1 = malloc(32);
    if (p1 == NULL) {
        printf("malloc(32) fallo\n");
        return;
    }
    printf("malloc(32) = ");
    printHex64((uint64_t)p1);
    printf("\n");
    void *p2 = malloc(64);
    if (p2 == NULL) {
        printf("malloc(64) fallo\n");
        free(p1);
        return;
    }
    printf("malloc(64) = ");
    printHex64((uint64_t)p2);
    printf("\n");
    printf("Liberando ");
    printHex64((uint64_t)p1);
    printf("...\n");
    free(p1);
    void *p3 = malloc(16);
    if (p3 == NULL) {
        printf("malloc(16) fallo\n");
        free(p2);
        return;
    }
    printf("malloc(16) = ");
    printHex64((uint64_t)p3);
    printf("\n");
    free(p2);
    free(p3);
    printf("=== Prueba completada ===\n");
}

void user_memchunks(void) {
    printf("=== Prueba de chunks consecutivos ===\n");
    void *ptrs[4];
    int i;
    for (i = 0; i < 4; i++) {
        ptrs[i] = malloc(4096);
        if (ptrs[i] == NULL) {
            printf("malloc(4096) fallo en el bloque %d\n", i+1);
            break;
        }
        printf("malloc(4096) bloque %d = ", i+1);
        printHex64((uint64_t)ptrs[i]);
        printf("\n");
    }
    for (int j = 0; j < i; j++) {
        free(ptrs[j]);
    }
    printf("=== Prueba completada ===\n");
}

void user_meminfo(void) {
    meminfo();
}

void foo() {
    while (1) {
        for(int i= 0; i<100000000; i++); // Retardo
        printf("Proceso foo (PID %d) ejecutandose...\n", my_getpid());
    }
}