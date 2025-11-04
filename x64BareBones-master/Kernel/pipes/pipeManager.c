#include "pipeManager.h"

struct PipeManagerCDT {
    PipeListADT pipeList;
    int max_write_fd;
    int max_read_fd;
    MemoryManagerADT memoryManager;
};

static PipeManagerADT globalPipeManager = NULL;

PipeManagerADT createPipeManager(MemoryManagerADT memoryManager) {
    PipeManagerADT pipeManager = (PipeManagerADT)allocateMemory(memoryManager, sizeof(struct PipeManagerCDT));
    pipeManager->pipeList = createPipeList(memoryManager);
    pipeManager->max_write_fd = 2; 
    pipeManager->max_read_fd = 2;  
    pipeManager->memoryManager = memoryManager;
    globalPipeManager = pipeManager;
    return pipeManager;
}

PipeManagerADT getGlobalPipeManager() {
    return globalPipeManager;
}

void destroyPipeManager(PipeManagerADT pipeManager, MemoryManagerADT memoryManager) {
    destroyPipeList(pipeManager->pipeList, memoryManager);
    freeMemory(memoryManager, pipeManager);
}

void addPipe(PipeManagerADT pipeManager, int fd[2]) {
    Pipe* newPipe = (Pipe*)allocateMemory(pipeManager->memoryManager, sizeof(Pipe));
    newPipe->read_fd = pipeManager->max_read_fd++;
    newPipe->write_fd = pipeManager->max_write_fd++;
    for (int i = 0; i < BUFFER_SIZE; i++) {
        newPipe->buffer[i] = 0;
    }
    addPipeToList(pipeManager->pipeList, newPipe, pipeManager->memoryManager);
    fd[0] = newPipe->read_fd;
    fd[1] = newPipe->write_fd;
}

int writeToPipe(PipeManagerADT pipeManager, int write_fd, uint64_t * data) {
    Pipe* pipe = getPipeFromListToWrite(pipeManager->pipeList, write_fd);
    if (pipe) {
        int i;
        for (i = 0; i < BUFFER_SIZE; i++) {
            if (pipe->buffer[i] == 0) {
                pipe->buffer[i] = data[i];
            }
        }
        return i;
    }
    return 0;
}


int readFromPipe(PipeManagerADT pipeManager, int read_fd, uint64_t * buffer, int size){
    Pipe* pipe = getPipeFromListToRead(pipeManager->pipeList, read_fd);
    if (pipe) {
        for (int i = 0; i < size && i < BUFFER_SIZE; i++) {
            buffer[i] = pipe->buffer[i];
            pipe->buffer[i] = 0; 
        }
        int j = 0;
        for (int i = 0; i < BUFFER_SIZE; i++) {
            if (pipe->buffer[i] != 0) {
                pipe->buffer[j++] = pipe->buffer[i];
            }
        }
        for (int i = j; i < BUFFER_SIZE; i++) {
            pipe->buffer[i] = 0;
        }
    }
    return size <= BUFFER_SIZE ? size : BUFFER_SIZE;
}

void closePipe(PipeManagerADT pipeManager, int fd[2]) {
    deletePipe(pipeManager->pipeList, fd[0], fd[1]);
}