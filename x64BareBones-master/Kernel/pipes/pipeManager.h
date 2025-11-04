#include "pipesList.h"

typedef struct PipeManagerCDT * PipeManagerADT;

PipeManagerADT createPipeManager(MemoryManagerADT memoryManager);

PipeManagerADT getGlobalPipeManager();

void destroyPipeManager(PipeManagerADT pipeManager, MemoryManagerADT memoryManager);

void addPipe(PipeManagerADT pipeManager, int fd[2]);

int writeToPipe(PipeManagerADT pipeManager, int write_fd, uint64_t *data);

int readFromPipe(PipeManagerADT pipeManager, int read_fd, uint64_t * buffer, int size);

void closePipe(PipeManagerADT pipeManager, int fd[2]);

