// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include "../include/userlib.h"
#include "../include/getTime.h"
#include "../include/test_util.h"

extern uint64_t getPid(void);

static uint64_t bcd_to_dec(uint8_t bcd) {
	return ((bcd >> 4) * 10ULL) + (bcd & 0x0FULL);
}

static uint64_t seconds_since(const Time *start, const Time *now) {
	uint64_t start_secs = bcd_to_dec(start->hours) * 3600ULL + bcd_to_dec(start->minutes) * 60ULL + bcd_to_dec(start->seconds);
	uint64_t now_secs = bcd_to_dec(now->hours) * 3600ULL + bcd_to_dec(now->minutes) * 60ULL + bcd_to_dec(now->seconds);
	if (now_secs >= start_secs) {
		return now_secs - start_secs;
	}
	return (24ULL * 3600ULL) - (start_secs - now_secs);
}

void loop(uint64_t argc, char **argv) {
	uint64_t wait_seconds = 1;
	if (argc > 0 && argv != NULL && argv[0] != NULL) {
		int64_t parsed = satoi(argv[0]);
		if (parsed > 0) {
			wait_seconds = (uint64_t)parsed;
		}
	}

	pid_t pid = getPid();
	Time start;
	getTime(&start);

	while (1) {
	printf("Hola! Soy el proceso %d\n", (int)pid);

		// Bloqueo activo simple usando time syscalls
		Time current;
		while (seconds_since(&start, &current) < wait_seconds) {
			getTime(&current);
		}

		start = current;
	}
}