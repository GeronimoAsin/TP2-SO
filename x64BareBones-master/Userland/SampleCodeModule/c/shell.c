#include <stdint.h>
#include "userlib.h"
#include "getTime.h"
#include "printRegisters.h"
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
	"- invalidOp: Genera una excepcion de operacion invalida\n";


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
	if (strcmp(cmd, "zeroDiv\n") == 0) return 8;
	if (strcmp(cmd, "invalidOp\n") == 0) return 9;
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
			case 8:
                 int a = 1;
                 int c = a / 0;
				//genera una excepcion de division por cero
				break;
			case 9:
                invalidOp();
				break;
            default:
                printf("Comando no encontrado. Escriba 'help' para ver los comandos disponibles.\n");
        }
        printf("\n");
    }
}
