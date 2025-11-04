#include "pipe.h"
#include "../memoryManager/memoryManager.h"

typedef struct PipeListCDT * PipeListADT ;

PipeListADT createPipeList(MemoryManagerADT memoryManager);

void destroyPipeList(PipeListADT pipeList, MemoryManagerADT memoryManager);

int addPipeToList(PipeListADT pipeList, Pipe* newPipe, MemoryManagerADT memoryManager);

Pipe* getPipeFromListToRead(PipeListADT pipeList, int read_fd);

Pipe* getPipeFromListToWrite(PipeListADT pipeList, int write_fd);

void deletePipe(PipeListADT pipeList, int read_fd, int write_fd);