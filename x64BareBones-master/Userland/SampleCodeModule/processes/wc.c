#include "../include/userlib.h"

extern uint64_t getPid(void);
extern int getReadFd(pid_t pid);
extern int getWriteFd(pid_t pid);
extern int read(int fd, char *buffer, int count);
extern int write(int fd, const char *buffer, int count);
extern void my_exit(void);

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

	int lines = 1;
	int lineHasContent = 0;
	int isKeyboard = (readFd == 0);

	while (1) {
		char ch;
		int bytes = read(readFd, &ch, 1);
		if (bytes == 0) {
			//EOF
			break;
		}

		if (isKeyboard && ch != '\n') {
			write(writeFd, &ch, 1);
		}

		if (ch == '\n') {
			if (isKeyboard) {
				write(writeFd, "\n", 1);
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

	printf("\nLineas: %d\n", lines);
	my_exit();
}
