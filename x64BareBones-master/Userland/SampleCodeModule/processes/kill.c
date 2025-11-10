// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include "../include/userlib.h"
#include "../include/test_util.h"

extern void my_exit(void);
extern int64_t my_kill(pid_t pid);

void kill(uint64_t argc, char **argv) {
	if (argc == 0 || argv == NULL || argv[0] == NULL) {
		printf("Uso: kill <pid>\n");
		my_exit();
	}

	int64_t parsed = satoi(argv[0]);
	if (parsed <= 1) {
		printf("PID invalido: %s\n", argv[0]);
		my_exit();
	}

	pid_t target = (pid_t)parsed;

	my_kill(target);
	printf("Proceso %d terminado\n", (int)target);

	my_exit();
}
