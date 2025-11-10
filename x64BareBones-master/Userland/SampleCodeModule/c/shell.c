#include <stdint.h>
#include "../include/userlib.h"
#include "../include/syscall.h"
#include "../include/processes.h"
#include "getTime.h"
#include "printRegisters.h"
extern void invalidOp();
#define CMD_MAX_CHARS 100
#define PROMPT "Shell $> "

typedef void (*ProcessEntry)(uint64_t, char **);

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



static int readLine(char *buffer, int max) {
    int i = 0;
    char c = 0;
    while (i < max - 1 && c != '\n') {
        char temp=0;
        read(0, &temp, 1); // Lee un carácter del teclado
        c = temp;

        if (c == '\r') continue; 
        if (c == '\b' || c == 127) {
            if (i > 0) {
                i--;
                deleteLastChar(); 
            }
        } else if ((c >= 32 && c <= 126) || c == '\n') { 
            buffer[i++] = c;
            if (c != '\n') {
                putChar(c); 
            }
        }
          if (i == max - 1) {
            clearCursor(); 
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
    int len = 0;
    while (cmd[len] != '\0') {
        len++;
    }
    int i = len - 1;

    while (i >= 0 && (cmd[i] == '\n' || cmd[i] == ' ' || cmd[i] == '\t')) {
        i--;
    }

    if (i >= 0 && cmd[i] == '&') {
        i--;
        while (i >= 0 && (cmd[i] == ' ' || cmd[i] == '\t')) {
            i--;
        }
        cmd[i + 1] = '\n';
        cmd[i + 2] = '\0';
        return 1;
    }
    
    return 0;
}

// Detecta si hay un pipe en el comando y separa los comandos
// Retorna 1 si hay pipe, 0 si no
// Si hay pipe, llena cmd1 y cmd2 con los comandos separados
static int hasPipe(const char *cmd, char *cmd1, char *cmd2, int maxLen) {
    int i = 0;
    int pipePos = -1;
    
    while (cmd[i] != '\0' && cmd[i] != '\n') {
        if (cmd[i] == '|') {
            pipePos = i;
            break;
        }
        i++;
    }

    if (pipePos == -1) {
        return 0;
    }

    int j = 0;
    i = 0;
    while (cmd[i] == ' ' || cmd[i] == '\t') {
        i++;
    }
    while (i < pipePos && j < maxLen - 2) {
        cmd1[j++] = cmd[i++];
    }
    while (j > 0 && (cmd1[j-1] == ' ' || cmd1[j-1] == '\t')) {
        j--;
    }
    cmd1[j++] = '\n';
    cmd1[j] = '\0';
    
    i = pipePos + 1;
    j = 0;
    while (cmd[i] == ' ' || cmd[i] == '\t') {
        i++;
    }
    while (cmd[i] != '\0' && cmd[i] != '\n' && j < maxLen - 2) {
        cmd2[j++] = cmd[i++];
    }
    while (j > 0 && (cmd2[j-1] == ' ' || cmd2[j-1] == '\t')) {
        j--;
    }
    cmd2[j++] = '\n';
    cmd2[j] = '\0';
    
    return 1;
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
    if (strncmp(cmd, "test_mm", 7) == 0) {
        char c = cmd[7];
        if (c == ' ' || c == '\t' || c == '\n' || c == '\0' || c == '+' || c == '-' || (c >= '0' && c <= '9')) return 10;
    }
    if (strcmp(cmd, "mem\n") == 0) return 11;
    if (strcmp(cmd, "getPid\n") == 0) return 13;
    if (strcmp(cmd, "ps\n") == 0) return 14;
    if (strcmp(cmd, "fg\n") == 0) return 15;
    if (strncmp(cmd, "loop", 4) == 0) {
        char c = cmd[4];
        if (c == '\n' || c == '\0' || c == ' ' || c == '\t' || c == '+' || c == '-' || (c >= '0' && c <= '9')) {
            return 24;
        }
    }
    if (strncmp(cmd, "kill", 4) == 0) {
        char c = cmd[4];
        if (c == ' ' || c == '\t' || (c >= '0' && c <= '9')) return 16;
    }
    if (strncmp(cmd, "nice", 4) == 0) {
        char c = cmd[4];
        if (c == ' ' || c == '\t' || (c >= '0' && c <= '9')) return 17;
    }
    if (strncmp(cmd, "block", 5) == 0) {
        char c = cmd[5];
        if (c == ' ' || c == '\t' || (c >= '0' && c <= '9')) return 18;
    }
    if (strncmp(cmd, "unblock", 7) == 0) {
        char c = cmd[7];
        if (c == ' ' || c == '\t' || (c >= '0' && c <= '9')) return 19;
    }
    if (strncmp(cmd, "test_processes", 14) == 0) {
        char c = cmd[14];
        if (c == ' ' || c == '\t' || c == '\n' || c == '\0' || (c >= '0' && c <= '9')) return 20;
    }
    if (strncmp(cmd, "test_prio", 9) == 0) {
        char c = cmd[9];
        if (c == ' ' || c == '\t' || c == '\n' || c == '\0' || (c >= '0' && c <= '9')) return 21;
    }
    if (strncmp(cmd, "test_synchro", 12) == 0) {
         char c = cmd[12];
         if (c == ' ' || c == '\t' || c == '\n' || c == '\0' || (c >= '0' && c <= '9')) return 22;
    }
    if (strcmp(cmd, "cat\n") == 0) return 25;
    if (strcmp(cmd, "wc\n") == 0) return 26;
    if (strcmp(cmd, "filter\n") == 0) return 27;
    if (strncmp(cmd, "mvar", 4) == 0) {
        char c = cmd[4];
        if (c == ' ' || c == '\t' || (c >= '0' && c <= '9')) return 28;
    }
    return -1;
}

// Helper para crear procesos con manejo de foreground/background
pid_t  createProcessAndWait(ProcessEntry entryPoint, char *name, int argc, char **argv, int bg) {
    pid_t pid = createProcess((void (*)(int, char**))entryPoint, name, argc, argv, !bg);
    if (!bg) {
        waitPid(pid);
    }
    return pid;
}

pid_t my_switch(int cmd, char *buffer, int bg) {
    switch (cmd) {
            case 0: // help
                return createProcessAndWait(&help, "help_process", 0, NULL, bg);
            case 2: // clear
                return createProcessAndWait(&clear, "clear_process", 0, NULL, bg);
            case 3: { // echo
                char *toPrint = buffer + 4;
                char * argv[1] = { toPrint };
                return createProcessAndWait(&echo, "echo_process", 1, argv, bg);
            }
            case 4: // time
                return createProcessAndWait(&time, "time_process", 0, NULL, bg);
            case 5: // registers
                return createProcessAndWait(&registers, "registers_process", 0, NULL, bg);
            case 10: { // test_mm 
                char *p = buffer;
                for (int k = 0; k < 7 && *p; k++) p++;
                
                while (*p == ' ' || *p == '\t') p++;
                
                char argbuf[32];
                int ai = 0;
                if (*p && *p != '\n' && *p != '\r' && *p != '&') {
                    while (*p && *p != ' ' && *p != '\t' && *p != '\n' && *p != '\r' && *p != '&' && ai < 31) {
                        argbuf[ai++] = *p++;
                    }
                    argbuf[ai] = '\0';
                    char *argv_local[1] = { argbuf };
                    return createProcessAndWait((ProcessEntry)&test_mm, "memtest_process", 1, argv_local, bg);
                } else {
                    printf("Error: test_mm requiere un argumento\n");
                    printf("Uso: test_mm <max_memory>\n");
                    printf("Ejemplo: test_mm 10240\n");
                }
                break;
            }
            case 11: //meminfo
                return createProcessAndWait(&mem, "mem_process", 0, NULL, bg);
            case 13: { // getPid
                return createProcessAndWait(&getMyPid, "getPid_process", 0, NULL, bg);
            }
            case 14: // ps
                return createProcessAndWait(&ps, "ps_process", 0, NULL, bg);
            case 15: { // fg
                return createProcessAndWait(&foreground, "fg_process", 0, NULL, bg);
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
                return createProcessAndWait(&kill, "kill_process", 1, argv_local, bg);
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

                return createProcessAndWait(&nice, "nice_process", argc_local, argv_local, bg);
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
                return createProcessAndWait(&block, "block_process", 1, argv_local, bg);
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
                return createProcessAndWait(&unblock, "unblock_process", 1, argv_local, bg);
            }
            case 20: { // test_processes <max_processes>
                char *p = buffer;
                for (int k = 0; k < 14 && *p; k++) p++;

                while (*p == ' ' || *p == '\t') p++;

                char argbuf[32];
                int ai = 0;
                if (*p && *p != '\n' && *p != '\r' && *p != '&') {
                    while (*p && *p != ' ' && *p != '\t' && *p != '\n' && *p != '\r' && *p != '&' && ai < 31) {
                        argbuf[ai++] = *p++;
                    }
                    argbuf[ai] = '\0';
                    
                    char *argv_local[1] = { argbuf };
                    return createProcessAndWait((ProcessEntry)&test_processes, "test_processes", 1, argv_local, bg);
                } else {
                    printf("Error: test_processes requiere un argumento\n");
                    printf("Uso: test_processes <max_processes>\n");
                    printf("Ejemplo: test_processes 5\n");
                }
                break;
            }
            case 21: { // test_prio <max_value>
                char *p = buffer;
                for (int k = 0; k < 9 && *p; k++) p++;
                
                // Saltar espacios
                while (*p == ' ' || *p == '\t') p++;

                char argbuf[32];
                int ai = 0;
                if (*p && *p != '\n' && *p != '\r' && *p != '&') {
                    while (*p && *p != ' ' && *p != '\t' && *p != '\n' && *p != '\r' && *p != '&' && ai < 31) {
                        argbuf[ai++] = *p++;
                    }
                    argbuf[ai] = '\0';
                    
                    char *argv_local[1] = { argbuf };
                    return createProcessAndWait((ProcessEntry)&test_prio, "test_prio", 1, argv_local, bg);
                } else {
                    printf("Error: test_prio requiere un argumento\n");
                    printf("Uso: test_prio <max_value>\n");
                    printf("Ejemplo: test_prio 100000\n");
                }
                break;
            }
            case 22: { // test_synchro <n> <use_sem>
                char *p = buffer;
                for (int k = 0; k < 12 && *p; k++) p++;
                
                while (*p == ' ' || *p == '\t') p++;
                
                char argbuf1[32];
                int ai1 = 0;
                if (*p && *p != '\n' && *p != '\r' && *p != '&') {
                    while (*p && *p != ' ' && *p != '\t' && *p != '\n' && *p != '\r' && *p != '&' && ai1 < 31) {
                        argbuf1[ai1++] = *p++;
                    }
                    argbuf1[ai1] = '\0';
                    
                    while (*p == ' ' || *p == '\t') p++;
                    
                    char argbuf2[32];
                    int ai2 = 0;
                    if (*p && *p != '\n' && *p != '\r' && *p != '&') {
                        while (*p && *p != ' ' && *p != '\t' && *p != '\n' && *p != '\r' && *p != '&' && ai2 < 31) {
                            argbuf2[ai2++] = *p++;
                        }
                        argbuf2[ai2] = '\0';
                        char *argv_local[2] = { argbuf1, argbuf2 };
                        return createProcessAndWait((ProcessEntry)&test_sync, "test_synchro", 2, argv_local, bg);
                    } else {
                        printf("Error: test_synchro requiere dos argumentos\n");
                        printf("Uso: test_synchro <n> <use_sem>\n");
                        printf("Ejemplo: test_synchro 1000 1 (con semaforo)\n");
                        printf("Ejemplo: test_synchro 1000 0 (sin semaforo)\n");
                    }
                } else {
                    printf("Error: test_synchro requiere dos argumentos\n");
                    printf("Uso: test_synchro <n> <use_sem>\n");
                    printf("Ejemplo: test_synchro 1000 1 (con semaforo)\n");
                    printf("Ejemplo: test_synchro 1000 0 (sin semaforo)\n");
                }
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

                return createProcessAndWait(&loop, "loop_process", argc_local, argv_local, bg);
            }
            case 25: { // cat
                return createProcessAndWait(&cat, "cat_process", 0, NULL, bg);
            }
            case 26: { // wc
                return createProcessAndWait(&wc, "wc_process", 0, NULL, bg);
            }
            case 27: { // filter
                return createProcessAndWait(&filter, "filter_process", 0, NULL, bg);
            }
            case 28: { // mvar <num_escritores> <num_lectores>
                char *p = buffer;
                for (int k = 0; k < 4 && *p; k++) p++;

                while (*p == ' ' || *p == '\t') p++;

                static char mvarArgs[2][16];
                char *argv_local[2] = { NULL, NULL };
                int argc_local = 0;
                for (int arg = 0; arg < 2; arg++) {
                    if (*p == '\0' || *p == '\n' || *p == '&') {
                        break;
                    }

                    int idx = 0;
                    while (*p && *p != ' ' && *p != '\t' && *p != '\n' && *p != '&' && idx < (int)(sizeof(mvarArgs[arg]) - 1)) {
                        mvarArgs[arg][idx++] = *p++;
                    }
                    mvarArgs[arg][idx] = '\0';
                    argv_local[arg] = mvarArgs[arg];
                    argc_local++;

                    while (*p == ' ' || *p == '\t') {
                        p++;
                    }
                }

                if (argc_local < 2) {
                    printf("Error: uso correcto -> mvar <num_escritores> <num_lectores>\n");
                    printf("Ejemplo: mvar 2 2\n");
                    break;
                }

                return createProcessAndWait(&mvar, "mvar_process", argc_local, argv_local, bg);
            }
            default:
                printf("Comando no encontrado. Escriba 'help' para ver los comandos disponibles.\n");
        }
        return -1;
}

static int resolvePipelineCommand(char *cmd, ProcessEntry *entry, int *argc, char ***argv) {
    int code = interpret(cmd);
    *argc = 0; *argv = NULL; *entry = NULL;
    switch (code) {
        case 0: // help
            *entry = &help; return 1;
        case 2: // clear
            *entry = &clear; return 1;
        case 3: { // echo <texto>
            *entry = &echo;
            char *p = cmd;
            for (int k = 0; k < 4 && *p; k++) p++;
            while (*p == ' ' || *p == '\t') p++;
            static char *echoArgv[1];
            echoArgv[0] = p; 
            *argv = echoArgv;
            *argc = 1;
            return 1;
        }
        case 4: // time
            *entry = &time; return 1;
        case 5: // registers
            *entry = &registers; return 1;
        case 10: { // test_mm con argumentos
            *entry = (ProcessEntry)&test_mm;
            char *p = cmd;
            for (int k = 0; k < 7 && *p; k++) p++;
            while (*p == ' ' || *p == '\t') p++;
            
            static char testmmArg[32];
            static char *testmmArgv[1];
            int ai = 0;
            if (*p && *p != '\n' && *p != '\r') {
                while (*p && *p != ' ' && *p != '\t' && *p != '\n' && *p != '\r' && ai < 31) {
                    testmmArg[ai++] = *p++;
                }
                testmmArg[ai] = '\0';
                testmmArgv[0] = testmmArg;
                *argv = testmmArgv;
                *argc = 1;
                return 1;
            }
            return 0; 
        }
        case 11: // mem
            *entry = &mem; return 1;
        case 13: // getPid
            *entry = &getMyPid; return 1;
        case 14: // ps
            *entry = &ps; return 1;
        case 15: // fg
            *entry = &foreground; return 1;
        case 16: { // kill <pid>
            *entry = &kill;
            char *p = cmd;
            for (int k = 0; k < 4 && *p; k++) p++;
            while (*p && (*p == ' ' || *p == '\t')) p++;
            
            static char killArg[16];
            static char *killArgv[1];
            int idx = 0;
            if (*p && *p != '\n' && *p != '\r') {
                while (*p && *p != ' ' && *p != '\t' && *p != '\n' && *p != '\r' && idx < 15) {
                    killArg[idx++] = *p++;
                }
                killArg[idx] = '\0';
                killArgv[0] = killArg;
                *argv = killArgv;
                *argc = 1;
                return 1;
            }
            return 0;
        }
        case 17: { // nice <pid> <priority>
            *entry = &nice;
            char *p = cmd;
            for (int k = 0; k < 4 && *p; k++) p++;
            while (*p == ' ' || *p == '\t') p++;
            
            static char niceArgs[2][16];
            static char *niceArgv[2];
            int argCount = 0;
            
            for (int arg = 0; arg < 2 && *p && *p != '\n' && *p != '\r'; arg++) {
                int idx = 0;
                while (*p && *p != ' ' && *p != '\t' && *p != '\n' && *p != '\r' && idx < 15) {
                    niceArgs[arg][idx++] = *p++;
                }
                niceArgs[arg][idx] = '\0';
                niceArgv[arg] = niceArgs[arg];
                argCount++;
                while (*p == ' ' || *p == '\t') p++;
            }
            
            if (argCount >= 2) {
                *argv = niceArgv;
                *argc = argCount;
                return 1;
            }
            return 0;
        }
        case 18: { // block <pid>
            *entry = &block;
            char *p = cmd;
            for (int k = 0; k < 5 && *p; k++) p++;
            while (*p == ' ' || *p == '\t') p++;
            
            static char blockArg[16];
            static char *blockArgv[1];
            int idx = 0;
            if (*p && *p != '\n' && *p != '\r') {
                while (*p && *p != ' ' && *p != '\t' && *p != '\n' && *p != '\r' && idx < 15) {
                    blockArg[idx++] = *p++;
                }
                blockArg[idx] = '\0';
                blockArgv[0] = blockArg;
                *argv = blockArgv;
                *argc = 1;
                return 1;
            }
            return 0;
        }
        case 19: { // unblock <pid>
            *entry = &unblock;
            char *p = cmd;
            for (int k = 0; k < 7 && *p; k++) p++;
            while (*p == ' ' || *p == '\t') p++;
            
            static char unblockArg[16];
            static char *unblockArgv[1];
            int idx = 0;
            if (*p && *p != '\n' && *p != '\r') {
                while (*p && *p != ' ' && *p != '\t' && *p != '\n' && *p != '\r' && idx < 15) {
                    unblockArg[idx++] = *p++;
                }
                unblockArg[idx] = '\0';
                unblockArgv[0] = unblockArg;
                *argv = unblockArgv;
                *argc = 1;
                return 1;
            }
            return 0;
        }
        case 20: { // test_processes <max_processes>
            *entry = (ProcessEntry)&test_processes;
            char *p = cmd;
            for (int k = 0; k < 14 && *p; k++) p++;
            while (*p == ' ' || *p == '\t') p++;
            
            static char testprocArg[32];
            static char *testprocArgv[1];
            int ai = 0;
            if (*p && *p != '\n' && *p != '\r') {
                while (*p && *p != ' ' && *p != '\t' && *p != '\n' && *p != '\r' && ai < 31) {
                    testprocArg[ai++] = *p++;
                }
                testprocArg[ai] = '\0';
                testprocArgv[0] = testprocArg;
                *argv = testprocArgv;
                *argc = 1;
                return 1;
            }
            return 0;
        }
        case 21: { // test_prio <max_value>
            *entry = (ProcessEntry)&test_prio;
            char *p = cmd;
            for (int k = 0; k < 9 && *p; k++) p++;
            while (*p == ' ' || *p == '\t') p++;
            
            static char testprioArg[32];
            static char *testprioArgv[1];
            int ai = 0;
            if (*p && *p != '\n' && *p != '\r') {
                while (*p && *p != ' ' && *p != '\t' && *p != '\n' && *p != '\r' && ai < 31) {
                    testprioArg[ai++] = *p++;
                }
                testprioArg[ai] = '\0';
                testprioArgv[0] = testprioArg;
                *argv = testprioArgv;
                *argc = 1;
                return 1;
            }
            return 0;
        }
        case 22: { // test_synchro <n> <use_sem>
            *entry = (ProcessEntry)&test_sync;
            char *p = cmd;
            for (int k = 0; k < 12 && *p; k++) p++;
            while (*p == ' ' || *p == '\t') p++;
            
            static char testsyncArgs[2][32];
            static char *testsyncArgv[2];
            int argCount = 0;
            
            for (int arg = 0; arg < 2 && *p && *p != '\n' && *p != '\r'; arg++) {
                int idx = 0;
                while (*p && *p != ' ' && *p != '\t' && *p != '\n' && *p != '\r' && idx < 31) {
                    testsyncArgs[arg][idx++] = *p++;
                }
                testsyncArgs[arg][idx] = '\0';
                testsyncArgv[arg] = testsyncArgs[arg];
                argCount++;
                while (*p == ' ' || *p == '\t') p++;
            }
            
            if (argCount >= 2) {
                *argv = testsyncArgv;
                *argc = argCount;
                return 1;
            }
            return 0;
        }
        case 24: { // loop [seconds]
            *entry = &loop;
            char *p = cmd;
            for (int k = 0; k < 4 && *p; k++) p++;
            while (*p == ' ' || *p == '\t') p++;
            
            static char loopArg[32];
            static char *loopArgv[1];
            
            if (*p && *p != '\n' && *p != '\r') {
                int idx = 0;
                while (*p && *p != ' ' && *p != '\t' && *p != '\n' && *p != '\r' && idx < 31) {
                    loopArg[idx++] = *p++;
                }
                loopArg[idx] = '\0';
                loopArgv[0] = loopArg;
                *argv = loopArgv;
                *argc = 1;
            } else {
                *argc = 0;
                *argv = NULL;
            }
            return 1;
        }
        case 25: // cat
            *entry = &cat; return 1;
        case 26: // wc
            *entry = &wc; return 1;
        case 27: // filter
            *entry = &filter; return 1;
        case 28: { // mvar <num_escritores> <num_lectores>
            *entry = &mvar;
            char *p = cmd;
            for (int k = 0; k < 4 && *p; k++) p++;
            while (*p == ' ' || *p == '\t') p++;
            
            static char mvarArgs[2][16];
            static char *mvarArgv[2];
            int argCount = 0;
            
            for (int arg = 0; arg < 2 && *p && *p != '\n' && *p != '\r'; arg++) {
                int idx = 0;
                while (*p && *p != ' ' && *p != '\t' && *p != '\n' && *p != '\r' && idx < 15) {
                    mvarArgs[arg][idx++] = *p++;
                }
                mvarArgs[arg][idx] = '\0';
                mvarArgv[arg] = mvarArgs[arg];
                argCount++;
                while (*p == ' ' || *p == '\t') p++;
            }
            
            if (argCount >= 2) {
                *argv = mvarArgv;
                *argc = argCount;
                return 1;
            }
            return 0;
        }
        default:
            return 0;
    }
}

void startShell() {
    printTime();
    printf("%s", ascii_art);
    char buffer[CMD_MAX_CHARS];
    while (1) {
        clearCursor();
        printf(PROMPT);
        drawCursor(); 
        readLine(buffer, CMD_MAX_CHARS);
        printf("\n");

        int bg = isBackground(buffer);
        char cmd1[CMD_MAX_CHARS];
        char cmd2[CMD_MAX_CHARS];
        if (hasPipe(buffer, cmd1, cmd2, CMD_MAX_CHARS)) {
            int fd[2];
            pipe(fd); 

            ProcessEntry entry1 = NULL; int argc1=0; char **argv1=NULL;
            ProcessEntry entry2 = NULL; int argc2=0; char **argv2=NULL;
            if (!resolvePipelineCommand(cmd1, &entry1, &argc1, &argv1)) {
                printf("Error: comando izquierdo del pipe no soportado en pipeline.\n");
                continue;
            }
            if (!resolvePipelineCommand(cmd2, &entry2, &argc2, &argv2)) {
                printf("Error: comando derecho del pipe no soportado en pipeline.\n");
                continue;
            }
            int foregroundFlag = 0;
            pid_t pid1 = createProcessWithFdsSys((void (*)(int, char**))entry1, "pipe_left", argc1, argv1, foregroundFlag, -1, fd[1]);
            pid_t pid2 = createProcessWithFdsSys((void (*)(int, char**))entry2, "pipe_right", argc2, argv2, foregroundFlag, fd[0], -1);

            if (!bg) {
                waitPid(pid1);
                waitPid(pid2);
            }
            closePipe(fd);
            printf("\n");
            continue;
        }
               
        int cmd = interpret(buffer);
        my_switch(cmd, buffer, bg);
        printf("\n");
    }
}