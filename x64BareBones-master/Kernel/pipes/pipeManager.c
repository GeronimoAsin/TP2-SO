// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
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

int writeToPipe(PipeManagerADT pipeManager, int write_fd, const char * data, int size) {
    Pipe* pipe = getPipeFromListToWrite(pipeManager->pipeList, write_fd);
    if (!pipe) {
        return 0;
    }
    int written = 0;
    for (int i = 0; i < BUFFER_SIZE && written < size; i++) {
        if (pipe->buffer[i] == 0) { 
            pipe->buffer[i] = data[written++];
        }
    }
    return written;
}

int readFromPipe(PipeManagerADT pipeManager, int read_fd, char * outBuffer, int size){
    Pipe* pipe = getPipeFromListToRead(pipeManager->pipeList, read_fd);
    if (!pipe) {
        return 0;
    }
    int read = 0;
    for (int i = 0; i < BUFFER_SIZE && read < size; i++) {
        char c = pipe->buffer[i];
        if (c == 0) {
            continue; 
        }
        outBuffer[read++] = c;
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
    return read;
}

void closePipe(PipeManagerADT pipeManager, int fd[2]) {
    deletePipe(pipeManager->pipeList, fd[0], fd[1]);
}