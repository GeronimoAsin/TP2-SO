#include "../include/userlib.h"

extern uint64_t getPid(void);
extern int getReadFd(pid_t pid);
extern int getWriteFd(pid_t pid);
extern int read(int fd, char *buffer, int count);
extern int write(int fd, const char *buffer, int count);
extern void my_exit(void);

static int isVowel(char c) {
	if (c >= 'A' && c <= 'Z') {
		c = (char)(c - 'A' + 'a');
	}
	return (c == 'a' || c == 'e' || c == 'i' || c == 'o' || c == 'u');
}

void filter(uint64_t argc, char **argv) {
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

	while (1) {
		char ch;
		int bytes = read(readFd, &ch, 1);
		if (bytes == 0) {
			break;
		}

		if (!isVowel(ch) || ch == '\n') {
			write(writeFd, &ch, 1);
		}

	}

	my_exit();
}
