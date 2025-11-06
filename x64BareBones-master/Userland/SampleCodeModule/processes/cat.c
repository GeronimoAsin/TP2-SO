#include "../include/userlib.h"

void cat(uint64_t argc, char **argv) {
	(void)argc;
	(void)argv;

	pid_t pid = getPid();
	int readFd = getReadFd(pid);
	int writeFd = getWriteFd(pid);
    printf("cat: readFd=%d, writeFd=%d\n", readFd, writeFd);

	if (readFd <= 0) {
		readFd = 0; // usar teclado si no hay descriptor asociado
	}
	if (writeFd <= 0) {
		writeFd = 1; // fallback a pantalla
	}

	char buffer;
	while (1) {
		int bytes = read(readFd, &buffer, 1);
		if (bytes <= 0) {
			if (readFd == 0) {
				continue; // stdin espera nuevos datos
			}
			break; // pipe/padre cerró el descriptor
		}

		write(writeFd, &buffer, 1);
		if (buffer == '\n') {
			break;
		}
	}

	my_exit();
}
