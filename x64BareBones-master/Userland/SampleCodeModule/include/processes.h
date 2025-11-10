#include <stdint.h>
#include "../include/userlib.h"

void echo(uint64_t argc, char **argv);
void help(uint64_t argc, char **argv);
void block(uint64_t argc, char **argv);
void unblock(uint64_t argc, char **argv);
void cat(uint64_t argc, char **argv);
void clear(uint64_t argc, char **argv);
void filter(uint64_t argc, char **argv);
void kill(uint64_t argc, char **argv);
void loop(uint64_t argc, char **argv);
void mem(uint64_t argc, char **argv);
void mvar(uint64_t argc, char **argv);
void nice(uint64_t argc, char **argv);
void ps(uint64_t argc, char **argv);
void registers(uint64_t argc, char **argv);
void time(uint64_t argc, char **argv);
void getMyPid(uint64_t argc, char **argv);
void foreground(uint64_t arcg, char **argv);
uint64_t test_mm(uint64_t argc, char *argv[]);
uint64_t test_sync(uint64_t argc, char *argv[]); 
uint64_t test_prio(uint64_t argc, char *argv[]);
uint64_t test_processes(uint64_t argc, char *argv[]);
void wc(uint64_t argc, char **argv);