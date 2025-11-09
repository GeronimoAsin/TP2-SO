#include "../include/userlib.h"
#include "../include/syscall.h"

static char *help_text =
    "Comandos disponibles:\n"
    "- help: Muestra esta ayuda\n"
    "- clear: Limpia la pantalla\n"
    "- echo + [texto]: Imprime el texto en pantalla\n"
    "- time: Muestra la hora actual\n"
    "- ps: Lista todos los procesos\n"
    "- foo: Ejecuta un proceso de ejemplo\n"
    "- mem: Muestra el estado actual de la memoria \n"
    "- fg: Trae el primer proceso en background a foreground\n"
    "- kill <pid>: Termina el proceso con el PID especificado\n"
    "- nice <pid> <priority>: Cambia la prioridad del proceso\n"
    "- block <pid>: Bloquea el proceso con el PID especificado\n"
    "- unblock <pid>: Desbloquea el proceso con el PID especificado\n"
    "- mvar <escritores> <lectores>: Demo de sincronizacion\n"
    "\nNota: Agrega '&' al final de un comando para ejecutarlo en background\n"
    "Ejemplo: foo & (ejecuta foo en background)\n";

void help(uint64_t argc, char **argv){
    write(getWriteFd(getPid()),help_text, strlen(help_text));
    my_exit();
}
