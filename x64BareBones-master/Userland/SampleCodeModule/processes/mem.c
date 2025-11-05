#include "../include/userlib.h"
#include "../include/syscall.h"

void mem(uint64_t argc, char **argv) {
	user_meminfo();
	my_exit();
}
