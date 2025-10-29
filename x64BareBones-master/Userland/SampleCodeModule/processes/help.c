static char *help_text =
    "Comandos disponibles:\n"
    "- help: Muestra esta ayuda\n"
    "- clear: Limpia la pantalla\n"
    "- echo + [texto]: Imprime el texto en pantalla\n"
    "- time: Muestra la hora actual\n";

void help(){
    printf("%s", help_text);
    exit();
}
