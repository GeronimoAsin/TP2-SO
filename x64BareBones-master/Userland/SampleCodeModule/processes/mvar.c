// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include "../include/userlib.h"
#include "../include/test_util.h"
#include <stdint.h>


#define SEM_EMPTY "mvar_empty"
#define SEM_FULL "mvar_full"
#define SEM_MUTEX "mvar_mutex"  // mutex de la variable compartida

extern void my_exit(void);
extern uint64_t my_sem_wait(char *sem_id);
extern uint64_t my_sem_post(char *sem_id);
extern uint64_t my_sem_open(char *sem_id, uint64_t initialValue);
extern pid_t createProcess(void (*start_routine)(int, char**), char *name, int argc, char **argv, int foreground);
extern void printCharWithColor(char c, uint32_t color);

static int parse_int(const char *str) {
    int result = 0;
    int i = 0;
    while (str[i] >= '0' && str[i] <= '9') {
        result = result * 10 + (str[i] - '0');
        i++;
    }
    return result;
}

static void random_wait() {
    uint32_t wait_time = GetUniform(500000) + 100000;
    bussy_wait(wait_time);
}

static volatile char shared_value = 0;

// WRITER
void writer_process(int argc, char **argv) {
    if (argc < 1) {
        my_exit();
        return;
    }

    int writer_id = parse_int(argv[0]);
    char write_char = 'A' + writer_id;

    random_wait();

    while (1) {
        random_wait();

        my_sem_wait(SEM_EMPTY);

 
        my_sem_wait(SEM_MUTEX);
        shared_value = write_char;
        my_sem_post(SEM_MUTEX);


        my_sem_post(SEM_FULL);
    }
}

// LECTOR
void reader_process(int argc, char **argv) {
    if (argc < 1) {
        my_exit();
        return;
    }

    int reader_id = parse_int(argv[0]);

    // colores
    uint32_t reader_colors[] = {
        0xFF0080,  // Rosa - Lector 0
        0x00FF80,  // Verde  - Lector 1
        0x0080FF,  // Azul  - Lector 2
        0xFF8000,  // Naranja  - Lector 3
        0x8000FF,  // Violeta - Lector 4
        0x00FFFF,  // Cyan  - Lector 5
        0xFFFF00,  // Amarillo  - Lector 6
        0xFF00FF   // Magenta  - Lector 7
    };

    uint32_t my_color = reader_colors[reader_id % 8];

    random_wait();

    while (1) {
        random_wait();

        my_sem_wait(SEM_FULL);

        my_sem_wait(SEM_MUTEX);
        char value_read = shared_value;
        my_sem_post(SEM_MUTEX);

        printCharWithColor(value_read, my_color);

        my_sem_post(SEM_EMPTY);
    }
}

static void int_to_str(int num, char *buf) {
    int i = 0;
    if (num == 0) {
        buf[i++] = '0';
        buf[i] = '\0';
        return;
    }

    char temp[16];
    int j = 0;
    while (num > 0) {
        temp[j++] = '0' + (num % 10);
        num /= 10;
    }

    while (j > 0) {
        buf[i++] = temp[--j];
    }
    buf[i] = '\0';
}

void mvar(uint64_t argc, char **argv) {

    if (argc < 2) {
        printf("Uso: mvar <num_escritores> <num_lectores>\n");
        printf("Ejemplo: mvar 2 2\n");
        return;
    }

    int num_writers = parse_int(argv[0]);
    int num_readers = parse_int(argv[1]);

    if (num_writers <= 0 || num_readers <= 0) {
        printf("Error: el numero de escritores y lectores debe ser mayor a 0\n");
        return;
    }

    if (num_writers > 26) {
        printf("Error: maximo 26 escritores (A-Z)\n");
        return;
    }


    // empty: MVar está vacía (disponible para escribir) - inicialmente 1
    // full: valor disponible (para leer) - inicialmente 0
    // mutex: mutex de acceso a la variable compartida - inicialmente 1
    my_sem_open(SEM_EMPTY, 1);
    my_sem_open(SEM_FULL, 0);
    my_sem_open(SEM_MUTEX, 1);


    static char writer_ids[26][16];
    static char reader_ids[10][16];

    for (int i = 0; i < num_writers; i++) {
        int_to_str(i, writer_ids[i]);

        static char writer_names[26][32];
        int name_idx = 0;
        writer_names[i][name_idx++] = 'w';
        writer_names[i][name_idx++] = 'r';
        writer_names[i][name_idx++] = 'i';
        writer_names[i][name_idx++] = 't';
        writer_names[i][name_idx++] = 'e';
        writer_names[i][name_idx++] = 'r';
        writer_names[i][name_idx++] = '_';
        writer_names[i][name_idx++] = 'A' + i;
        writer_names[i][name_idx] = '\0';

        char *writer_argv[1] = { writer_ids[i] };
        createProcess((void (*)(int, char**))&writer_process, writer_names[i], 1, writer_argv, 0);
    }

    for (int i = 0; i < num_readers; i++) {
        int_to_str(i, reader_ids[i]);

        static char reader_names[10][32];
        int name_idx = 0;
        reader_names[i][name_idx++] = 'r';
        reader_names[i][name_idx++] = 'e';
        reader_names[i][name_idx++] = 'a';
        reader_names[i][name_idx++] = 'd';
        reader_names[i][name_idx++] = 'e';
        reader_names[i][name_idx++] = 'r';
        reader_names[i][name_idx++] = '_';

        if (i >= 10) {
            reader_names[i][name_idx++] = '0' + (i / 10);
        }
        reader_names[i][name_idx++] = '0' + (i % 10);
        reader_names[i][name_idx] = '\0';

        char *reader_argv[1] = { reader_ids[i] };
        createProcess((void (*)(int, char**))&reader_process, reader_names[i], 1, reader_argv, 0);
    }

    printf("MVar iniciado: %d escritores y %d lectores ejecutandose\n", num_writers, num_readers);
    printf("Los escritores escriben letras (A,B,C...), los lectores las leen e imprimen con su color\n");
    printf("Use 'ps' para ver procesos y 'kill <pid>' para detenerlos\n\n");

    // El proceso principal termina inmediatamente después de crear los procesos
    // Los procesos hijos continúan ejecutándose de forma independiente
	my_exit();
}

