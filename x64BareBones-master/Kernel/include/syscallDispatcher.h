#ifndef SYSCALLDISPATCHER_H
#define SYSCALLDISPATCHER_H
#include <stdint.h>

uint64_t sys_read(uint64_t fd, char *buff, uint64_t count);
uint64_t sys_write(uint64_t fd, const char *buffer, uint64_t count); 
uint64_t syscallDispatcher(uint64_t id, ...);





#endif