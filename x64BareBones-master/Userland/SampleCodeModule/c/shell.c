#include <stdint.h>
#include "userlib.h"
#include "getTime.h"
#include "printRegisters.h"
#include <unistd.h>
extern void syscall(uint64_t rax, uint64_t rbx, uint64_t rdx, uint64_t rcx);
extern void invalidOp();
#define CMD_MAX_CHARS 100
#define PROMPT "Shell $> "

void echo(uint64_t argc, char **argv);
void help(uint64_t argc, char **argv);
void block(uint64_t argc, char **argv);
void unblock(uint64_t argc, char **argv);
void cat(uint64_t argc, char **argv);
void clear(uint64_t argc, char **argv);
void filter(uint64_t argc, char **argv);
void kill(uint64_t argc, char **argv);
void loop(uint64_t argc, char **argv);
void mem(uint64_t argc, char **argv);
void mvar(uint64_t argc, char **argv);
void nice(uint64_t argc, char **argv);
void ps(uint64_t argc, char **argv);
void registers(uint64_t argc, char **argv);
void time(uint64_t argc, char **argv);
void getMyPid(uint64_t argc, char **argv);
void foreground(uint64_t arcg, char **argv);
uint64_t test_mm(uint64_t argc, char *argv[]);
uint64_t test_no_sync(uint64_t argc, char *argv[]);
uint64_t test_sync(uint64_t argc, char *argv[]);
uint64_t test_prio(uint64_t argc, char *argv[]);
uint64_t test_processes(uint64_t argc, char *argv[]);
void wc(uint64_t argc, char **argv);

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
    if (strcmp(cmd, "memchunks\n") == 0) return 7;
	if (strcmp(cmd, "zeroDiv\n") == 0) return 8;
	if (strcmp(cmd, "invalidOp\n") == 0) return 9;
    // Acepta "test_mm" seguido de espacio/tab/newline, signo o dígito, o incluso sin separador antes del número
    if (strncmp(cmd, "test_mm", 7) == 0) {
        char c = cmd[7];
        if (c == ' ' || c == '\t' || c == '\n' || c == '\0' || c == '+' || c == '-' || (c >= '0' && c <= '9')) return 10;
    }
    if (strcmp(cmd, "meminfo\n") == 0) return 11;
    if (strcmp(cmd, "foo\n") == 0) return 12;
    if (strcmp(cmd, "getPid\n") == 0) return 13;
    if (strcmp(cmd, "ps\n") == 0) return 14;
    if (strcmp(cmd, "fg\n") == 0) return 15;
    if (strncmp(cmd, "loop", 4) == 0) {
        char c = cmd[4];
        if (c == '\n' || c == '\0' || c == ' ' || c == '\t' || c == '+' || c == '-' || (c >= '0' && c <= '9')) {
            return 24;
        }
    }
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
    // test_processes
    if (strncmp(cmd, "test_processes", 14) == 0) {
        char c = cmd[14];
        if (c == ' ' || c == '\t' || c == '\n' || c == '\0' || (c >= '0' && c <= '9')) return 20;
    }
    // test_prio
    if (strncmp(cmd, "test_prio", 9) == 0) {
        char c = cmd[9];
        if (c == ' ' || c == '\t' || c == '\n' || c == '\0' || (c >= '0' && c <= '9')) return 21;
    }
    // test_synchro
    if (strncmp(cmd, "test_synchro", 12) == 0) {
        char c = cmd[12];
        if (c == ' ' || c == '\t' || c == '\n' || c == '\0' || (c >= '0' && c <= '9')) return 22;
    }
    // test_no_synchro
    if (strncmp(cmd, "test_no_synchro", 15) == 0) {
        char c = cmd[15];
        if (c == ' ' || c == '\t' || c == '\n' || c == '\0' || (c >= '0' && c <= '9')) return 23;
    }
    return -1;
}

// Helper para crear procesos con manejo de foreground/background
static void createProcessAndWait(void (*entryPoint)(int, char**), char *name, int argc, char **argv, int bg) {
    pid_t pid = createProcess(entryPoint, name, argc, argv, !bg);
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
                createProcessAndWait(&help, "help_process", 0, NULL, bg);
                break;
            case 2: // clear
                createProcessAndWait(&clear, "clear_process", 0, NULL, bg);
                break;
            case 3: { // echo
                const char *toPrint = buffer + 4;
                char * argv[2] = { "1", toPrint };
                createProcessAndWait(&echo, "echo_process", 2, argv, bg);
                break;
            }
            case 4: // time
                createProcessAndWait(&time, "time_process", 0, NULL, bg);
                break;
            case 5: // registers
                createProcessAndWait(&registers, "registers_process", 0, NULL, bg);
                break;
            case 7: // memchunks
                user_memchunks();
                break;
            case 10: { // test_mm con argumentos
                // Parsear el argumento opcional (max_memory)
                char *p = buffer;
                // Avanzar hasta después de "test_mm" (7 caracteres)
                for (int k = 0; k < 7 && *p; k++) p++;
                
                // Saltar espacios
                while (*p == ' ' || *p == '\t') p++;
                
                // Si hay un argumento, parsearlo
                char argbuf[32];
                int ai = 0;
                if (*p && *p != '\n' && *p != '\r' && *p != '&') {
                    // Copiar el argumento
                    while (*p && *p != ' ' && *p != '\t' && *p != '\n' && *p != '\r' && *p != '&' && ai < 31) {
                        argbuf[ai++] = *p++;
                    }
                    argbuf[ai] = '\0';
                    
                    // Crear array de argumentos
                    char *argv_local[1] = { argbuf };
                    createProcessAndWait((void (*)(int, char**))&test_mm, "memtest_process", 1, argv_local, bg);
                } else {
                    // Sin argumentos - mostrar mensaje de ayuda
                    printf("Error: test_mm requiere un argumento\n");
                    printf("Uso: test_mm <max_memory>\n");
                    printf("Ejemplo: test_mm 10240\n");
                }
                break;
            }
            case 11: //meminfo
                createProcessAndWait(&user_meminfo, "meminfo_process", 0, NULL, bg);
                break;
            case 12: // foo
                createProcessAndWait(&foo, "foo_process", 0, NULL, bg);
                break;
            case 13: { // getPid
                createProcessAndWait(&getMyPid, "getPid_process", 0, NULL, bg);
                break;
            }
            case 14: // ps
                createProcessAndWait(&ps, "ps_process", 0, NULL, bg);
                break;
            case 15: { // fg
                createProcessAndWait(&foreground, "fg_process", 0, NULL, bg);
                break;
            }
            case 16: { // kill
                char *p = buffer;
                for (int k = 0; k < 4 && *p; k++) {
                    p++;
                }
                while (*p && (*p == ' ' || *p == '\t')) {
                    p++;
                }

                if (*p == '\0' || *p == '\n' || !(*p >= '0' && *p <= '9')) {
                    printf("Error: uso correcto -> kill <pid>\n");
                    break;
                }

                pid_t pidToKill = 0;
                while (*p >= '0' && *p <= '9') {
                    pidToKill = pidToKill * 10 + (*p - '0');
                    p++;
                }

                while (*p == ' ' || *p == '\t') {
                    p++;
                }
                if (*p != '\0' && *p != '\n' && *p != '&') {
                    printf("Error: uso correcto -> kill <pid>\n");
                    break;
                }

                pid_t currentPid = getPid();
                if (pidToKill == currentPid) {
                    printf("Error: no puedes matar la shell\n");
                    break;
                }

                static char killArg[16];
                int len = 0;
                pid_t tempPid = pidToKill;
                if (tempPid == 0) {
                    killArg[len++] = '0';
                } else {
                    char rev[16];
                    int r = 0;
                    while (tempPid > 0 && r < (int)(sizeof(rev))) {
                        rev[r++] = (char)('0' + (tempPid % 10));
                        tempPid /= 10;
                    }
                    while (r > 0 && len < (int)(sizeof(killArg) - 1)) {
                        killArg[len++] = rev[--r];
                    }
                }
                killArg[len] = '\0';

                char *argv_local[1] = { killArg };
                createProcessAndWait(&kill, "kill_process", 1, argv_local, bg);
                break;
            }
            case 17: { // nice <pid> <priority>
                char *p = buffer;
                for (int k = 0; k < 4 && *p; k++) {
                    p++;
                }
                while (*p == ' ' || *p == '\t') {
                    p++;
                }

                static char niceArgs[2][16];
                char *argv_local[2] = { NULL, NULL };
                int argc_local = 0;

                for (int arg = 0; arg < 2; arg++) {
                    if (*p == '\0' || *p == '\n' || *p == '&') {
                        break;
                    }

                    int idx = 0;
                    while (*p && *p != ' ' && *p != '\t' && *p != '\n' && *p != '&' && idx < (int)(sizeof(niceArgs[arg]) - 1)) {
                        niceArgs[arg][idx++] = *p++;
                    }
                    niceArgs[arg][idx] = '\0';
                    argv_local[arg] = niceArgs[arg];
                    argc_local++;

                    while (*p == ' ' || *p == '\t') {
                        p++;
                    }
                }

                if (argc_local < 2) {
                    printf("Error: uso correcto -> nice <pid> <priority>\n");
                    break;
                }

                createProcessAndWait(&nice, "nice_process", argc_local, argv_local, bg);
                break;
            }
            case 18: { // block <pid>
                char *p = buffer;
                for (int k = 0; k < 5 && *p; k++) {
                    p++;
                }
                while (*p == ' ' || *p == '\t') {
                    p++;
                }

                if (*p == '\0' || *p == '\n' || *p == '&') {
                    printf("Error: uso correcto -> block <pid>\n");
                    break;
                }

                static char blockArg[16];
                int idx = 0;
                while (*p && *p != ' ' && *p != '\t' && *p != '\n' && *p != '&' && idx < (int)(sizeof(blockArg) - 1)) {
                    blockArg[idx++] = *p++;
                }
                blockArg[idx] = '\0';

                char *argv_local[1] = { blockArg };
                createProcessAndWait(&block, "block_process", 1, argv_local, bg);
                break;
            }
            case 19: { // unblock <pid>
                char *p = buffer;
                for (int k = 0; k < 7 && *p; k++) {
                    p++;
                }
                while (*p == ' ' || *p == '\t') {
                    p++;
                }

                if (*p == '\0' || *p == '\n' || *p == '&') {
                    printf("Error: uso correcto -> unblock <pid>\n");
                    break;
                }

                static char unblockArg[16];
                int idx = 0;
                while (*p && *p != ' ' && *p != '\t' && *p != '\n' && *p != '&' && idx < (int)(sizeof(unblockArg) - 1)) {
                    unblockArg[idx++] = *p++;
                }
                unblockArg[idx] = '\0';

                char *argv_local[1] = { unblockArg };
                createProcessAndWait(&unblock, "unblock_process", 1, argv_local, bg);
                break;
            }
            case 20: { // test_processes
                //createProcessAndWait((void (*)(int, char**))&test_processes, "test_processes", 0, NULL, bg);
                break;
            }
            case 21: { // test_prio
                //createProcessAndWait((void (*)(int, char**))&test_prio, "test_prio", 0, NULL, bg);
                break;
            }
            case 22: { // test_synchro
                //createProcessAndWait((void (*)(int, char**))&test_sync, "test_synchro", 0, NULL, bg);
                break;
            }
            case 23: { 
                //createProcessAndWait((void (*)(int, char**))&test_no_synchro, "test_no_synchro", 0, NULL, bg);
                break;
            }
            case 24: { // loop
                char *p = buffer;
                for (int k = 0; k < 4 && *p; k++) p++;
                while (*p == ' ' || *p == '\t') {
                    p++;
                }

                char *argv_local[1] = { NULL };
                int argc_local = 0;

                if (*p && *p != '\n' && *p != '&') {
                    static char loopArg[32];
                    int idx = 0;
                    while (*p && *p != ' ' && *p != '\t' && *p != '\n' && *p != '&' && idx < (int)(sizeof(loopArg) - 1)) {
                        loopArg[idx++] = *p++;
                    }
                    loopArg[idx] = '\0';
                    argv_local[0] = loopArg;
                    argc_local = 1;
                }

                createProcessAndWait(&loop, "loop_process", argc_local, argv_local, bg);
                break;
            }
            default:
                printf("Comando no encontrado. Escriba 'help' para ver los comandos disponibles.\n");
        }
        printf("\n");
    }
}