#include <stdint.h>
#include "userlib.h"
#include "getTime.h"
#include "printRegisters.h"
#include <unistd.h>
extern uint64_t test_mm(uint64_t argc, char *argv[]);
extern void syscall(uint64_t rax, uint64_t rbx, uint64_t rdx, uint64_t rcx);
extern void invalidOp();
#define CMD_MAX_CHARS 100
#define PROMPT "Shell $> "

extern void help();

static const char *ascii_art =
"+====================================================+\n"
"   ____  _          _ _                             \n"
"  / ___|| |__   ___| | |                            \n"
"  \\___ \\| '_ \\ / _ \\ | | Bienvenido a BatataOS 2.0! \n"
"   ___) | | | |  __/ | |                            \n"
"  |____/|_| |_|\\___|_|_|                           \n"
"                                                    \n"
"  >>> Listo para ejecutar sus comandos! <<<         \n"
"+====================================================+\n";

static void print(const char *str) {
    while (*str) {
        putChar(*str++);
    }
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
    print(buf);
}

static int readLine(char *buffer, int max) {
    int i = 0;
    char c = 0;
    while (i < max - 1 && c != '\n') {
        char temp=0;
        read(0, &temp, 1); // Lee un carácter del teclado
        c = temp;

        if (c == '\r') continue; // Ignora carriage return
        if (c == '\b' || c == 127) { // Maneja backspace
            if (i > 0) {
                i--;
                deleteLastChar(); // Borra visualmente el carácter
            }
        } else if ((c >= 32 && c <= 126) || c == '\n') { // Solo caracteres imprimibles y salto de línea
            buffer[i++] = c;
            if (c != '\n') {
                putChar(c); // Muestra el carácter en pantalla mientras se escribe
            }
        }
          if (i == max - 1) {
            clearCursor(); // clearCursor syscall cuando el buffer está lleno
        }
    }
    buffer[i] = 0;
    if (c != '\n') putChar('\n');
    return i;
}

// Verifica si el comando termina con '&' (background)
// Retorna 1 si es background, 0 si no lo es
// Modifica el buffer eliminando el '&' y espacios previos
static int isBackground(char *cmd) {
    // Calcular longitud
    int len = 0;
    while (cmd[len] != '\0') {
        len++;
    }
    
    int i = len - 1;
    
    // Retroceder desde el final saltando '\n' y espacios
    while (i >= 0 && (cmd[i] == '\n' || cmd[i] == ' ' || cmd[i] == '\t')) {
        i--;
    }
    
    // Si encontramos '&', lo removemos
    if (i >= 0 && cmd[i] == '&') {
        // Retroceder para eliminar espacios antes del '&'
        i--;
        while (i >= 0 && (cmd[i] == ' ' || cmd[i] == '\t')) {
            i--;
        }
        // Terminar el string después del último carácter válido
        cmd[i + 1] = '\n';
        cmd[i + 2] = '\0';
        return 1;
    }
    
    return 0;
}

static int interpret(const char *cmd) {
    if (strcmp(cmd, "help\n") == 0) return 0;
    if (strcmp(cmd, "clear\n") == 0) return 2;
    if (strncmp(cmd, "echo\n", 4) == 0 && (cmd[4] == ' ' || cmd[4] == '\t' || cmd[4] == 0)) return 3;
    if (strcmp(cmd, "time\n") == 0) return 4;
    if (strcmp(cmd, "registers\n") == 0) return 5;
    if (strcmp(cmd, "memtest\n") == 0) return 6;
	if (strcmp(cmd, "zeroDiv\n") == 0) return 8;
	if (strcmp(cmd, "invalidOp\n") == 0) return 9;
    if (strcmp(cmd, "memchunks\n") == 0) return 7;
    if (strcmp(cmd, "meminfo\n") == 0) return 11;
    if (strcmp(cmd, "foo\n") == 0) return 12;
    if (strcmp(cmd, "getPid\n") == 0) return 13;
    if (strcmp(cmd, "ps\n") == 0) return 14;
    if (strcmp(cmd, "fg\n") == 0) return 15;
    // Acepta "kill" seguido de espacio y un número
    if (strncmp(cmd, "kill", 4) == 0) {
        char c = cmd[4];
        if (c == ' ' || c == '\t' || (c >= '0' && c <= '9')) return 16;
    }
    // Acepta "nice" seguido de espacio y números
    if (strncmp(cmd, "nice", 4) == 0) {
        char c = cmd[4];
        if (c == ' ' || c == '\t' || (c >= '0' && c <= '9')) return 17;
    }
    // Acepta "block" seguido de espacio y un número
    if (strncmp(cmd, "block", 5) == 0) {
        char c = cmd[5];
        if (c == ' ' || c == '\t' || (c >= '0' && c <= '9')) return 18;
    }
    // Acepta "unblock" seguido de espacio y un número
    if (strncmp(cmd, "unblock", 7) == 0) {
        char c = cmd[7];
        if (c == ' ' || c == '\t' || (c >= '0' && c <= '9')) return 19;
    }
    // Acepta "test_mm" seguido de espacio/tab/newline, signo o dígito, o incluso sin separador antes del número
    if (strncmp(cmd, "test_mm", 7) == 0) {
        char c = cmd[7];
        if (c == ' ' || c == '\t' || c == '\n' || c == '\0' || c == '+' || c == '-' || (c >= '0' && c <= '9')) return 10;
    }
    return -1;
}

// Helper para crear procesos con manejo de foreground/background
static void createProcessAndWait(void (*entryPoint)(int, char**), char *name, int bg) {
    pid_t pid = createProcess(entryPoint, name, 0, NULL, !bg);
    if (!bg) {
        waitPid(pid);
    }
}

void startShell() {
    beep();
    printTime();
    printf("%s", ascii_art);
    char buffer[CMD_MAX_CHARS];
    while (1) {
        clearCursor(); // Limpia cualquier cursor previo
        printf(PROMPT);
        drawCursor(); // drawCursor DESPUÉS de imprimir el prompt
        readLine(buffer, CMD_MAX_CHARS);
        printf("\n");
        
        // Verificar si es comando en background
        int bg = isBackground(buffer);
        
        int cmd = interpret(buffer);
        switch (cmd) {
            case 0: // help
                createProcessAndWait(&help, "help_process", bg);
                break;
            case 2: // clear screen
                user_clear();
                break;
            case 3: { // echo
                const char *toPrint = buffer + 4;
                user_echo(toPrint);
                break;
            }
            case 4: // time
                user_time();
                break;
            case 5: // registers
                user_registers();
                break;
            case 6: // memtest
                user_memtest();
                break;
            case 7: // memchunks
                user_memchunks();
                break;
            case 8: {
                 int a = 1;
                 int c = a / 0;
                break;
            }
            case 9:
                invalidOp();
                break;
            case 10: { // test_mm
                // Buscar el primer token numérico tras el comando "test_mm"
                char *p = buffer;
                // Avanzar hasta el final del nombre "test_mm" (7 caracteres)
                for (int k = 0; k < 7 && *p; k++) p++;
                //numero
                while (*p && *p != '\n' && *p != '\r' && !((*p >= '0' && *p <= '9') || *p == '+' || *p == '-')) p++;

                // Si terminamos o encontramos newline sin número, error
                if (*p == '\0' || *p == '\n' || *p == '\r') {
                    printf("Error: uso correcto -> test_mm <max_memory>\n");
                    break;
                }

                // Copiar el token numérico (incluye signo opcional)
                char argbuf[32];
                int ai = 0;
                if (*p == '+' || *p == '-') {
                    argbuf[ai++] = *p++;
                }
                while (*p >= '0' && *p <= '9' && ai < (int)(sizeof(argbuf) - 1)) {
                    argbuf[ai++] = *p++;
                }
                argbuf[ai] = '\0';

                if (ai == 0 || (ai == 1 && (argbuf[0] == '+' || argbuf[0] == '-'))) {
                    printf("Error: uso correcto -> test_mm <max_memory>\n");
                    break;
                }

                // Llamar a test_mm
                char *argv_local[1] = { argbuf };
                printf("=== Test de stress de memoria (test_mm) ===\n");
                uint64_t res = test_mm(1, argv_local);
                if (res == (uint64_t)-1) {
                    printf("Error: uso correcto -> test_mm <max_memory>\n");
                }
                break;
            }
            case 11:
                user_meminfo();
                break;
            case 12: // foo
                createProcessAndWait(&foo, "foo_process", bg);
                break;
            case 13: { // getPid
                uint64_t pid = getPid();
                printf("PID del proceso actual: %d\n", pid);
                break;
            }
            case 14: // ps
                printProcesses();
                break;
            case 15: { // fg
                pid_t fgPid = fg();
                if (fgPid == -1) {
                    printf("No hay procesos en background\n");
                } else {
                    printf("Proceso %d traido a foreground\n", fgPid);
                }
                break;
            }
            case 16: { // kill
                // Parsear el PID del comando "kill <pid>"
                char *p = buffer;
                // Avanzar hasta después de "kill"
                for (int k = 0; k < 4 && *p; k++) p++;
                // Saltar espacios
                while (*p && (*p == ' ' || *p == '\t')) p++;
                
                // Si no hay número, error
                if (*p == '\0' || *p == '\n' || !(*p >= '0' && *p <= '9')) {
                    printf("Error: uso correcto -> kill <pid>\n");
                    break;
                }
                
                // Parsear el PID
                pid_t pidToKill = 0;
                while (*p >= '0' && *p <= '9') {
                    pidToKill = pidToKill * 10 + (*p - '0');
                    p++;
                }
                
                // Verificar que no intente matar la shell
                pid_t currentPid = getPid();
                if (pidToKill == currentPid) {
                    printf("Error: no puedes matar la shell\n");
                    break;
                }
                
                printf("Matando proceso con PID %d...\n", pidToKill);
                my_kill(pidToKill);
                printf("Proceso terminado\n");
                break;
            }
            case 17: { // nice <pid> <priority>
                // Parsear el comando "nice <pid> <priority>"
                char *p = buffer;
                for (int k = 0; k < 4 && *p; k++) p++; // Saltar "nice"
                while (*p && (*p == ' ' || *p == '\t')) p++;
                
                if (*p == '\0' || *p == '\n' || !(*p >= '0' && *p <= '9')) {
                    printf("Error: uso correcto -> nice <pid> <priority>\n");
                    break;
                }
                
                // Parsear el PID
                pid_t pidToNice = 0;
                while (*p >= '0' && *p <= '9') {
                    pidToNice = pidToNice * 10 + (*p - '0');
                    p++;
                }
                
                // Saltar espacios
                while (*p && (*p == ' ' || *p == '\t')) p++;
                
                if (*p == '\0' || *p == '\n' || !(*p >= '0' && *p <= '9')) {
                    printf("Error: uso correcto -> nice <pid> <priority>\n");
                    break;
                }
                
                // Parsear la prioridad
                uint64_t newPriority = 0;
                while (*p >= '0' && *p <= '9') {
                    newPriority = newPriority * 10 + (*p - '0');
                    p++;
                }
                
                printf("Cambiando prioridad del proceso %d a %d...\n", pidToNice, newPriority);
                my_nice(pidToNice, newPriority);
                printf("Prioridad cambiada\n");
                break;
            }
            case 18: { // block
                // Parsear el PID del comando "block <pid>"
                char *p = buffer;
                for (int k = 0; k < 5 && *p; k++) p++; // Saltar "block"
                while (*p && (*p == ' ' || *p == '\t')) p++;
                
                if (*p == '\0' || *p == '\n' || !(*p >= '0' && *p <= '9')) {
                    printf("Error: uso correcto -> block <pid>\n");
                    break;
                }
                
                pid_t pidToBlock = 0;
                while (*p >= '0' && *p <= '9') {
                    pidToBlock = pidToBlock * 10 + (*p - '0');
                    p++;
                }
                
                printf("Bloqueando proceso con PID %d...\n", pidToBlock);
                my_block(pidToBlock);
                printf("Proceso bloqueado\n");
                break;
            }
            case 19: { // unblock
                // Parsear el PID del comando "unblock <pid>"
                char *p = buffer;
                for (int k = 0; k < 7 && *p; k++) p++; // Saltar "unblock"
                while (*p && (*p == ' ' || *p == '\t')) p++;
                
                if (*p == '\0' || *p == '\n' || !(*p >= '0' && *p <= '9')) {
                    printf("Error: uso correcto -> unblock <pid>\n");
                    break;
                }
                
                pid_t pidToUnblock = 0;
                while (*p >= '0' && *p <= '9') {
                    pidToUnblock = pidToUnblock * 10 + (*p - '0');
                    p++;
                }
                
                printf("Desbloqueando proceso con PID %d...\n", pidToUnblock);
                my_unblock(pidToUnblock);
                printf("Proceso desbloqueado\n");
                break;
            }
            default:
                printf("Comando no encontrado. Escriba 'help' para ver los comandos disponibles.\n");
        }
        printf("\n");
    }
}