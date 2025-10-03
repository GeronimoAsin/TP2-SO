#ifndef MEMORY_MANAGER_H
#define MEMORY_MANAGER_H

typedef struct MemoryManagerCDT * MemoryManagerADT;

MemoryManagerADT createMemoryManager();

void * allocateMemory(MemoryManagerADT memoryManager, unsigned int size);

void freeMemory(MemoryManagerADT memoryManager, void *ptr);

void destroyMemoryManager(MemoryManagerADT memoryManager);

#endif // MEMORY_MANAGER_H