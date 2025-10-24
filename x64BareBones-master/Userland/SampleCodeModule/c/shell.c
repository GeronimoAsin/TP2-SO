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

static char *help_text =
    "Comandos disponibles:\n"
    "- help: Muestra esta ayuda\n"
    "- clear: Limpia la pantalla\n"
    "- echo + [texto]: Imprime el texto en pantalla\n"
    "- time: Muestra la hora actual\n"
    "- registers: Muestra los registros\n"
	"- zeroDiv: Genera una excepcion de division por cero\n"
	"- invalidOp: Genera una excepcion de operacion invalida\n"
    "- memtest: Test de asignacion y liberacion de memoria del memManager\n"
    "- memchunks: Test de asignacion de varios chunks consecutivos de memoria\n"
    "- test_mm: Test de stress del memory manager de la catedra (requiere un argumento de cantidad maxima de memoria)\n";


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
    // Acepta "test_mm" seguido de espacio/tab/newline, signo o dígito, o incluso sin separador antes del número
    if (strncmp(cmd, "test_mm", 7) == 0) {
        char c = cmd[7];
        if (c == ' ' || c == '\t' || c == '\n' || c == '\0' || c == '+' || c == '-' || (c >= '0' && c <= '9')) return 10;
    }
    return -1;
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
        int cmd = interpret(buffer);
        switch (cmd) {
            case 0: // help
                printf(help_text);
                break;
            case 2: // clear screen
                clearScreen();
                break;
            case 3: { // echo
                // Imprime lo que sigue después de "echo "
                const char *toPrint = buffer + 4;
                while (*toPrint == ' ' || *toPrint == '\t')
                toPrint++;
                printf(toPrint);
                break;
            }
            case 4: { // time
                printTime();
                break;
            }
            case 5: { // registers
                print_registers();
                break;
            }
            case 6: { // memtest
                printf("=== Prueba de memoria ===\n");

                // Primera asignación
                void *p1 = malloc(32);
                if (p1 == NULL) {
                    printf("malloc(32) fallo\n");
                    break;
                }
                printf("malloc(32) = ");
                printHex64((uint64_t)p1);
                printf("\n");

                // Segunda asignación
                void *p2 = malloc(64);
                if (p2 == NULL) {
                    printf("malloc(64) fallo\n");
                    free(p1);
                    break;
                }
                printf("malloc(64) = ");
                printHex64((uint64_t)p2);
                printf("\n");

                // Libero primera asignación
                printf("Liberando ");
                printHex64((uint64_t)p1);
                printf("...\n");
                free(p1);

                // Nueva asignación
                void *p3 = malloc(16);
                if (p3 == NULL) {
                    printf("malloc(16) fallo\n");
                    free(p2);
                    break;
                }
                printf("malloc(16) = ");
                printHex64((uint64_t)p3);
                print("\n");

                // Limpieza
                free(p2);
                free(p3);
                printf("=== Prueba completada ===\n");
                break;
            }
            case 7: { // memchunks
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
                // Liberar los bloques asignados
                for (int j = 0; j < i; j++) {
                    free(ptrs[j]);
                }
                printf("=== Prueba completada ===\n");
                break;
            }
            case 8: {
                 int a = 1;
                 int c = a / 0;
				//genera una excepcion de division por cero
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
            default:
                printf("Comando no encontrado. Escriba 'help' para ver los comandos disponibles.\n");
        }
        printf("\n");
    }
}
