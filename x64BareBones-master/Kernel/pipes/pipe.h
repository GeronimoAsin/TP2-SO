#include <stdint.h>

#define BUFFER_SIZE 1024

typedef struct Pipe{
    int write_fd;
    int read_fd;
    char buffer[BUFFER_SIZE];
}Pipe;