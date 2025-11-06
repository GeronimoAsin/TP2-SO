#include "userlib.h"
#include <stdint.h>
#include <stdarg.h>
#include <stddef.h>
#include "../include/memoryInfo.h"

#define CHUNK_SIZE 4096


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




int strlen(const char *str) {
    int length = 0;
    while (str[length] != '\0') {
        length++;
    }
    return length;
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
            if (*format == 'l' && *(format+1) == 'l') {
                if (*(format+2) == 'x') {
                    format += 2;
                    uint64_t num = va_arg(args, uint64_t);
                    char hex_buffer[17];
                    unsigned_numtohex64(num, hex_buffer);
                    const char *str = hex_buffer;
                    while (*str) {
                        char buffer[2] = {*str++, '\0'};
                        write(1, buffer, 1);
                    }
                } else if (*(format+2) == 'u') {
                    format += 2;
                    uint64_t num = va_arg(args, uint64_t);
                    char num_buffer[21]; // Buffer para uint64_t max (20 dígitos + '\0')
                    int num_len = 0;
                    if (num == 0) {
                        num_buffer[num_len++] = '0';
                    } else {
                        while (num > 0) {
                            num_buffer[num_len++] = (num % 10) + '0';
                            num /= 10;
                        }
                    }
                    while (num_len > 0) {
                        char buffer[2] = {num_buffer[--num_len], '\0'};
                        write(1, buffer, 1);
                    }
                } else {
                    format++;
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
    if (fd == 0) {
        my_sem_wait("waiting_to_read");
    }
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

uint64_t getPid() {
    return syscall(14, 0, 0, 0, 0, 0);
}

pid_t createProcess(void (*start_routine)(int, char**),char * name, int argc, char **argv, int foreground) {
    pid_t pid = syscall(13, (uint64_t)start_routine, foreground, (uint64_t)name, argc, (uint64_t)argv);
    return pid;
}

typedef struct ProcessInfo {
    pid_t pid;
    char *name;
    size_t state;
    size_t priority;
    uint64_t *stackPointer;
    pid_t parentPid;
    size_t foreground;
} ProcessInfo;


void printProcesses() {
    size_t count;
    ProcessInfo *processes = syscall(15, &count, 0, 0, 0, 0);
    for (size_t i = 0; i < count; i++) {
        char *stateStr;
        if(processes[i].state == 0) {
            stateStr = "BLOCKED";
        } else if(processes[i].state == 1) {
            stateStr = "READY";
        } else if(processes[i].state == 2) {
            stateStr = "RUNNING";
        } else if(processes[i].state == 3) {
            stateStr = "TERMINATED";
        }
        printf("PID: %d, Name: %s, State: %s, Priority: %d, Parent PID: %d, Foreground: %d\n",
               processes[i].pid,
               processes[i].name,
               stateStr,
               processes[i].priority,
               processes[i].parentPid,
               processes[i].foreground);
    }
    free(processes);
}

pid_t fg() {
    return (pid_t) syscall(20, 0, 0, 0, 0, 0);
}

void my_kill(pid_t pid) {
    syscall(21, (uint64_t)pid, 0, 0, 0, 0);
}

int64_t my_nice(uint64_t pid, uint64_t newPrio) {
    return syscall(22, pid, newPrio, 0, 0, 0);
}

int64_t my_block(uint64_t pid) {
    return syscall(23, pid, 0, 0, 0, 0);
}

int64_t my_unblock(uint64_t pid) {
    return syscall(24, pid, 0, 0, 0, 0);
}

int64_t my_yield() {
    return syscall(25, 0, 0, 0, 0, 0);
}

void meminfo(void) {
    MemoryInfo *kernelInfo = (MemoryInfo *)syscall(12, 0, 0, 0, 0, 0);
    if (kernelInfo == NULL) {
        printf("No se pudo obtener información de memoria.\n");
        return;
    }

    MemoryInfo info = *kernelInfo; // tomar snapshot local
    uint64_t usedChunks = 0;
    uint64_t freeChunks = 0;
    if (info.chunkSize != 0) {
        usedChunks = (info.usedBytes + info.chunkSize - 1) / info.chunkSize;
        if (info.chunkCount > usedChunks) {
            freeChunks = info.chunkCount - usedChunks;
        } else {
            freeChunks = (info.freeBytes + info.chunkSize - 1) / info.chunkSize;
        }
    }

    uint64_t usedPercent = info.heapSize ? (info.usedBytes * 100) / info.heapSize : 0;
    uint64_t freePercent = info.heapSize ? (info.freeBytes * 100) / info.heapSize : 0;

    printf("--- Estado de la memoria ---\n");
    printf("Direccion base del heap: 0x%llx\n", (unsigned long long)info.heapStart);
    printf("Dimension total del heap: %llu bytes\n", (unsigned long long)info.heapSize);
    printf("Dimension de chunk: %llu bytes\n", (unsigned long long)info.chunkSize);
    printf("Cantidad total de chunks: %llu\n", (unsigned long long)info.chunkCount);
    printf("Chunks usados: %llu\n", (unsigned long long)usedChunks);
    printf("Chunks libres: %llu\n", (unsigned long long)freeChunks);
    printf("Memoria usada: %llu bytes\n", (unsigned long long)info.usedBytes);
    printf("Memoria libre: %llu bytes\n", (unsigned long long)info.freeBytes);
    printf("Uso de memoria: %llu%% usados / %llu%% libres\n", (unsigned long long)usedPercent, (unsigned long long)freePercent);
    printf("Asignaciones totales: %llu\n", (unsigned long long)info.totalAllocations);
    printf("Liberaciones totales: %llu\n", (unsigned long long)info.totalFrees);
    printf("Asignaciones fallidas: %llu\n", (unsigned long long)info.failedAllocations);
	printf("----------------------------\n");
}

void user_meminfo(void) {
    meminfo();
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

void foo() {
    printf("Proceso foo (PID %d) ejecutandose...\n", getPid());
    printProcesses();
    for(int i= 0; i<100000000000; i++){ // Retardo
        
    }
    printf("END\n");
    my_exit();
}

void waitPid(pid_t pid) {
    syscall(18, (uint64_t)pid, 0, 0, 0, 0);
}

void my_exit() {
    pid_t currentPid = getPid();
    syscall(19, (uint64_t)currentPid, 0, 0, 0, 0);
}

uint64_t my_sem_open(char *sem_id, uint64_t initialValue) {
  return syscall(26, (uint64_t)sem_id, initialValue, 0, 0, 0);
}

uint64_t my_sem_wait(char *sem_id) {
  return syscall(27, (uint64_t)sem_id, 0, 0, 0, 0);
}

uint64_t my_sem_post(char *sem_id) {
  return syscall(28, (uint64_t)sem_id, 0, 0, 0, 0);
}

uint64_t my_sem_close(char *sem_id) {
  return syscall(29, (uint64_t)sem_id, 0, 0, 0, 0);
}

void setWriteFd(pid_t pid, int write_fd) {
    syscall(30, (uint64_t)pid, (uint64_t)write_fd, 0, 0, 0);
}

void setReadFd(pid_t pid, int read_fd) {
    syscall(31, (uint64_t)pid, (uint64_t)read_fd, 0, 0, 0);
}

int getWriteFd(pid_t pid) {
    return (int)syscall(34, (uint64_t)pid, 0, 0, 0, 0);
}

int getReadFd(pid_t pid) {
    return (int)syscall(35, (uint64_t)pid, 0, 0, 0, 0);
}

void createProcessAndWaitWithPipeW(void (*entryPoint)(int, char**), char *name, int argc, char **argv, int bg, int write_fd) {
    pid_t pid = createProcess(entryPoint, name, argc, argv, !bg);
    setWriteFd(pid, write_fd);
    if (!bg) {
        waitPid(pid);
    }
}

void createProcessAndWaitWithPipeR(void (*entryPoint)(int, char**), char *name, int argc, char **argv, int bg, int read_fd) {
    pid_t pid = createProcess(entryPoint, name, argc, argv, !bg);
    setReadFd(pid, read_fd);
    if (!bg) {
        waitPid(pid);
    }
}

void pipe(int fd[2]) {
    syscall(32, (uint64_t)fd, 0, 0, 0, 0);
}

void closePipe(int fd[2]) {
    syscall(33, (uint64_t)fd, 0, 0, 0, 0);
}