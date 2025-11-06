#include "../include/userlib.h"

void wc(uint64_t argc, char **argv) {
	(void)argc;
	(void)argv;

	pid_t pid = getPid();
	int readFd = getReadFd(pid);
	int writeFd = getWriteFd(pid);

	if (readFd <= 0 || readFd == 1) {
		readFd = 0;
	}
	if (writeFd <= 0) {
		writeFd = 1;
	}

	int lines = 0;
	int lineHasContent = 0;
	int isKeyboard = (readFd == 0);

	while (1) {
		char ch;
		int bytes = read(readFd, &ch, 1);
		if (bytes <= 0) {
			if (isKeyboard) {
				continue;
			}
			break;
		}

		if (isKeyboard && ch != '\n') {
			write(writeFd, &ch, 1);
		}

		if (ch == '\n') {
			if (isKeyboard) {
				write(writeFd, "\n", 1);
				if (!lineHasContent) {
					break;
				}
			}
			lines++;
			lineHasContent = 0;
			continue;
		}

		lineHasContent = 1;
	}

	if (!isKeyboard && lineHasContent) {
		lines++;
	}

	printf("Lineas: %d\n", lines);
	my_exit();
}
