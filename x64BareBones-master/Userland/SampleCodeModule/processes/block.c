#include "../include/userlib.h"
#include "../include/test_util.h"

extern void my_exit(void);
extern int64_t my_block(pid_t pid);
extern int64_t my_unblock(pid_t pid);

static int parse_pid(uint64_t argc, char **argv, pid_t *target, const char *usage_msg) {
	if (argc < 1 || argv == NULL || argv[0] == NULL) {
		printf("%s\n", usage_msg);
		return 0;
	}

	int64_t parsed = satoi(argv[0]);
	if (parsed <= 0) {
		printf("PID invalido: %s\n", argv[0]);
		return 0;
	}

	*target = (pid_t)parsed;
	return 1;
}

static int protect_shell(pid_t target) {
	if (target == 1 || target == 0) {
		printf("Error: no se puede matar a la shell ni al idle\n");
		return 0;
	}
	return 1;
}

void block(uint64_t argc, char **argv) {
	pid_t target;
	if (!parse_pid(argc, argv, &target, "Uso: block <pid>") || !protect_shell(target)) {
		my_exit();
	}
	int64_t result = my_block(target);
	if (result > 0) {
		printf("Proceso %d bloqueado\n", (int)target);
	} else {
		printf("No se pudo bloquear el proceso %d\n", (int)target);
	}

	my_exit();
}

void unblock(uint64_t argc, char **argv) {
	pid_t target;
	if (!parse_pid(argc, argv, &target, "Uso: unblock <pid>") || !protect_shell(target)) {
		my_exit();
	}

	int64_t result = my_unblock(target);
	if (result > 0) {
		printf("Proceso %d desbloqueado\n", (int)target);
	} else {
		printf("No se pudo desbloquear el proceso %d\n", (int)target);
	}

	my_exit();
}
