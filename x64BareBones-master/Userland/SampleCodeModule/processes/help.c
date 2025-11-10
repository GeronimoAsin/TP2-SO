#include "../include/userlib.h"
#include "../include/syscall.h"

static char *help_text =
    "Comandos disponibles:\n"
    "- clear: Limpia la pantalla\n"
    "- echo + [texto]: Imprime el texto en pantalla\n"
    "- time: Muestra la hora actual\n"
    "- help: Muestra esta ayuda\n"
    "- registers: Muestra el estado de los registros CPU la última vez que se oprimió ESC\n"
    "- mem: Imprime el estado de la memoria\n"
    "- ps: Lista todos los procesos\n"
    "- loop <secs>: Imprime un saludo cada secs segundos\n"
    "- fg: Trae el primer proceso en background a foreground\n"
    "- kill <pid>: Termina el proceso con el PID especificado\n"
    "- nice <pid> <priority>: Cambia la prioridad del proceso\n"
    "- block <pid>: Bloquea el proceso con el PID especificado\n"
    "- unblock <pid>: Desbloquea el proceso con el PID especificado\n"
    "- cat: Imprime lo que recibe por entrada estandar\n"
    "- wc: Cuenta cantidad de lineas de la entrada estandar\n"
    "- filter: Filtra las vocales de la entrada estandar\n"
    "- mvar <escritores> <lectores>: Implementa un problema de lectores y escritores\n"
    "- test_mm <max_memory>: Test de manejo de memoria\n"
    "- test_processes <num_processes>: Test de creación de procesos\n"
    "- test_prio <final_value>: Test de prioridades\n"
    "- test_synchro <num_processes> <synchro>: Test de sincronización con semáforos (synchro = 1) o no (synchro = 0)\n"
    "\nNota: Agregar '&' al final de un comando para ejecutarlo en background\n"
    "Ejemplo: loop & (ejecuta loop en background)\n";

void help(uint64_t argc, char **argv){
    write(getWriteFd(getPid()),help_text, strlen(help_text));
    my_exit();
}
