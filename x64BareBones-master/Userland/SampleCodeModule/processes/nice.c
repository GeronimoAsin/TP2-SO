// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include "../include/userlib.h"
#include "../include/test_util.h"

extern void my_exit(void);
extern int64_t my_nice(pid_t pid, uint64_t newPrio);

void nice(uint64_t argc, char **argv) {
	if (argc < 2 || argv == NULL || argv[0] == NULL || argv[1] == NULL) {
		printf("Uso: nice <pid> <prioridad>\n");
		my_exit();
	}

	int64_t pidParsed = satoi(argv[0]);
	int64_t prioParsed = satoi(argv[1]);

	if (pidParsed <= 0) {
		printf("PID invalido: %s\n", argv[0]);
		my_exit();
	}

	if (prioParsed < 0) {
		printf("Prioridad invalida: %s\n", argv[1]);
		my_exit();
	}

	pid_t target = (pid_t)pidParsed;
	uint64_t newPriority = (uint64_t)prioParsed;

	printf("Cambiando prioridad de %d a %d...\n", (int)target, (int)newPriority);
	my_nice(target, newPriority);
	printf("Prioridad cambiada\n");

	my_exit();
}
