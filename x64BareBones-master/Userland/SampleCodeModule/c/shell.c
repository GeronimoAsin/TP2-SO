#include <stdint.h>
#include "userlib.h"
#include "getTime.h"
#include "printRegisters.h"
extern void syscall(uint64_t rax, uint64_t rbx, uint64_t rdx, uint64_t rcx);
#define CMD_MAX_CHARS 100
#define PROMPT "Shell $> "

static char *help_text =
    "Comandos disponibles:\n"
    "- help: Muestra esta ayuda\n"
    "- pongisGolf: Inicia el juego PongisGolf\n"
    "- clear: Limpia la pantalla\n"
    "- echo + [texto]: Imprime el texto en pantalla\n"
    "- time: Muestra la hora actual\n"
    "- registers: Muestra los registros\n"
	"- increase: Aumenta el tamano de la fuente\n"
	"- decrease: Disminuye el tamano de la fuente\n";


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
        syscall(0, 0, (uint64_t)&temp, 1); // Lee un carácter del teclado
        c = temp;

        if (c == '\r') continue; // Ignora carriage return
        if (c == '\b' || c == 127) { // Maneja backspace
            if (i > 0) {
                i--;
            }
        } else if ((c >= 32 && c <= 126) || c == '\n') { // Solo caracteres imprimibles y salto de línea
            buffer[i++] = c;
        }
          if (i == max - 1) {
            syscall(10, 0, 0, 0); // clearCursor syscall cuando el buffer está lleno
        }
    }
    buffer[i] = 0;
    if (c != '\n') putChar('\n');
    return i;
}

static int interpret(const char *cmd) {
    if (strcmp(cmd, "help\n") == 0) return 0;
    if (strcmp(cmd, "pongisGolf\n") == 0) return 1;
    if (strcmp(cmd, "clear\n") == 0) return 2;
    if (strncmp(cmd, "echo\n", 4) == 0 && (cmd[4] == ' ' || cmd[4] == '\t' || cmd[4] == 0)) return 3;
    if (strcmp(cmd, "time\n") == 0) return 4;
    if (strcmp(cmd, "registers\n") == 0) return 5;
	if (strcmp(cmd, "increase\n") == 0) return 6;
	if (strcmp(cmd, "decrease\n") == 0) return 7;
    return -1;
}

void startShell() {
    syscall(6,0,0,0);
    printTime();
    char buffer[CMD_MAX_CHARS];
    printf("Bienvenido a la shell! \n");
    while (1) {
        printf(PROMPT);
        syscall(9, 0, 0, 0); // drawCursor DESPUÉS de imprimir el prompt
        readLine(buffer, CMD_MAX_CHARS);
        printf("\n");
        int cmd = interpret(buffer);
        switch (cmd) {
            case 0: // help
                printf(help_text);
                break;
            case 1: // pongisGolf
                printf("Iniciando PongisGolf...\n");
                // pongisGolfMain();
                break;
            case 2: // clear screen
                //llama sys_clear
                syscall(2, 0, 0, 0);
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
                //print_registers();
                break;
            }
			case 6:
				syscall(7, 0, 0, 0); // increase font size
				break;
			case 7:
				syscall(8, 0, 0, 0); // decrease font size
				break;
            default:
                printf("Comando no encontrado. Escriba 'help' para ver los comandos disponibles.\n");
        }
        printf("\n");
    }
}
