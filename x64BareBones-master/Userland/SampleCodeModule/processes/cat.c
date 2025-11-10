#include "../include/userlib.h"

extern uint64_t getPid(void);
extern int getReadFd(pid_t pid);
extern int getWriteFd(pid_t pid);
extern void my_exit(void);
extern int read(int fd, char *buffer, int count);
extern int write(int fd, const char *buffer, int count);

#define CAT_LINE_BUFFER 256

void cat(uint64_t argc, char **argv) {
	(void)argc;
	(void)argv;

	pid_t pid = getPid();
	int readFd = getReadFd(pid);
	int writeFd = getWriteFd(pid);


	char line[CAT_LINE_BUFFER];
	int lineIndex = 0;

	while (1) {
		char ch;
		int bytes = read(readFd, &ch, 1);
		if (bytes == 0) {
			// EOF
			break;
		}

		if (ch == '\n') {
			write(writeFd, &ch, 1); // echo del enter
			if (lineIndex > 0) {
				write(writeFd, line, lineIndex);
			}
			write(writeFd, "\n", 1);
			lineIndex = 0;
			continue;
		}

		write(writeFd, &ch, 1); // mostrar el carácter a medida que se escribe
		if (lineIndex < CAT_LINE_BUFFER - 1) {
			line[lineIndex++] = ch;
		} else {
			// Línea demasiado larga: volcar lo acumulado y reiniciar
			write(writeFd, "\n", 1);
			write(writeFd, line, lineIndex);
			write(writeFd, "\n", 1);
			line[0] = ch;
			lineIndex = 1;
		}
	}

	if (lineIndex > 0) {
		write(writeFd, "\n", 1);
		write(writeFd, line, lineIndex);
		write(writeFd, "\n", 1);
	}

	my_exit();
}
