#include <stdint.h>
#include "userlib.h"
#include "getTime.h"
extern void syscall(uint64_t rax, uint64_t rbx, uint64_t rdx, uint64_t rcx);
#define CMD_MAX_CHARS 100
#define PROMPT "Shell $> "

static char *help_text =
    "Comandos disponibles:\n"
    "- help: Muestra esta ayuda\n"
    "- pongisGolf: Inicia el juego PongisGolf\n"
    "- clear: Limpia la pantalla\n"
    "- echo [texto]: Imprime el texto\n";

static void print(const char *str) {
    while (*str) {
        putChar(*str++);
    }
}

static int readLine(char *buffer, int max) {
    int i = 0;
    char c = 0;
    while (i < max - 1 && c != '\n') {
        // syscall 0 = read, fd=0, buffer, count=1
        char temp[1];
        syscall(0, 0, (uint64_t)temp, 1);
        c = temp[0];
        if (c == '\r') continue; // ignore carriage return
        if (c == '\b' || c == 127) { // backspace
            if (i > 0) {
                i--;
                print("\b \b");
            }
        } else if (c != '\n') {
            buffer[i++] = c;
            putChar(c);
        }
    }
    buffer[i] = 0;
    putChar('\n');
    return i;
}

static int interpret(const char *cmd) {
    if (strcmp(cmd, "help") == 0) return 0;
    if (strcmp(cmd, "pongisGolf") == 0) return 1;
    if (strcmp(cmd, "clear") == 0) return 2;
    if (strncmp(cmd, "echo", 4) == 0 && (cmd[4] == ' ' || cmd[4] == '\t' || cmd[4] == 0)) return 3;
    return -1;
}

void startShell() {
    printTime();
    char buffer[CMD_MAX_CHARS];
    print("Bienvenido a la shell! \n");
    while (1) {
        print(PROMPT);
        readLine(buffer, CMD_MAX_CHARS);

        int cmd = interpret(buffer);
        switch (cmd) {
            case 0: // help
                print(help_text);
                break;
            case 1: // pongisGolf
                print("Iniciando PongisGolf...\n");
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
                print(toPrint);
                break;
            }
            default:
                print("Comando no encontrado. Escriba 'help' para ver los comandos disponibles.\n");
        }
        print("\n");
    }
}
