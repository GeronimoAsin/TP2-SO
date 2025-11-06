#include "../include/userlib.h"

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

		if (!isVowel(ch) || ch == '\n') {
			write(writeFd, &ch, 1);
		}

		if (ch == '\n' && isKeyboard) {
			break;
		}
	}

	my_exit();
}
