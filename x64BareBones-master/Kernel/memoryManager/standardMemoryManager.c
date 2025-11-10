#include "memoryManager.h"
#include <stdint.h>
#include <stddef.h>
#include "../../Userland/SampleCodeModule/include/memoryInfo.h"

static MemoryManagerCDT memoryManagerInstance;
static uint64_t totalAllocations = 0;
static uint64_t totalFrees = 0;
static uint64_t failedAllocations = 0;

MemoryManagerADT createMemoryManager()
{
    static int initialized = 0;
    MemoryManagerADT newMem = &memoryManagerInstance;

    if (initialized) {
        return newMem;
    }

    uintptr_t alignedStart = ALIGN_POINTER(HEAP_START, WORD_ALIGN);
    newMem->heapStart = (uint8_t *)(uintptr_t)alignedStart; 

    newMem->heapSize = HEAP_SIZE - (unsigned int)(alignedStart - HEAP_START);
    newMem->chunkSize = CHUNK_SIZE;
    newMem->chunkCount = newMem->heapSize / CHUNK_SIZE;
    newMem->nextFreeIndex = newMem->chunkCount; // Comienza al final del stack

    // Inicializar la pila de chunks libres
    for (unsigned int i = 0; i < newMem->chunkCount; i++)
    {
        newMem->freeChunkStack[i] = newMem->heapStart + (i * newMem->chunkSize);
    }

    initialized = 1;
    return newMem;
}

void * allocateMemory(MemoryManagerADT memoryManager, size_t size) {
    if (memoryManager == NULL || size == 0) {
        // no existe el memory manager o el size es 0
        failedAllocations++;
        return NULL;
    }

    // Al pedir más que un chunk, retorna NULL (no se puede satisfacer con el standard mem manager)
    if (size > memoryManager->chunkSize) {
        failedAllocations++;
        return NULL;
    }

    // Verificar si hay chunks disponibles (stack no vacío)
    if (memoryManager->nextFreeIndex == 0) {
        failedAllocations++;
        return NULL; // No hay chunks libres
    }

    // Pop del stack y devuelve el inicio del chunk
    uint8_t *chunk = memoryManager->freeChunkStack[--memoryManager->nextFreeIndex];
    totalAllocations++;
    return (void *)chunk;
}

void freeMemory(MemoryManagerADT memoryManager, void *ptr) {
    if (memoryManager == NULL || ptr == NULL) {
        return;
    }

    uintptr_t start = (uintptr_t)memoryManager->heapStart;
    uintptr_t end = start + (memoryManager->chunkCount * memoryManager->chunkSize);
    uintptr_t p = (uintptr_t)ptr;

    if (p < start || p >= end) {
        return;
    }

    // Verificar que no desbordemos el stack
    if (memoryManager->nextFreeIndex >= memoryManager->chunkCount) {
        return; // Stack lleno, no se puede liberar más
    }

    // inicio del chunk al que pertenece el puntero
    uintptr_t chunkIndex = (p - start) / memoryManager->chunkSize;
    uint8_t *chunkStart = memoryManager->heapStart + (chunkIndex * memoryManager->chunkSize);

    // Push al stack: agregar el chunk en nextFreeIndex y luego incrementar
    memoryManager->freeChunkStack[memoryManager->nextFreeIndex] = chunkStart;
    memoryManager->nextFreeIndex++;
    totalFrees++;
}

void destroyMemoryManager(MemoryManagerADT memoryManager) {
    if (memoryManager == NULL) {
        return;
    }
    // Reinicio del stack al estado inicial
    memoryManager->nextFreeIndex = memoryManager->chunkCount;
}


MemoryInfo *meminfo(void)
{
    if (memoryManagerInstance.heapStart == NULL || memoryManagerInstance.heapSize == 0) {
        createMemoryManager();
    }

    MemoryManagerADT mm = &memoryManagerInstance;
    static MemoryInfo info;

    uint64_t usedChunks = (uint64_t)(mm->chunkCount - mm->nextFreeIndex);
    uint64_t freeChunks = (uint64_t)mm->nextFreeIndex;

    info.heapStart = (uint64_t)mm->heapStart;
    info.heapSize = mm->heapSize;
    info.chunkSize = mm->chunkSize;
    info.chunkCount = mm->chunkCount;
    info.usedBytes = usedChunks * mm->chunkSize;
    info.freeBytes = freeChunks * mm->chunkSize;
    info.totalAllocations = totalAllocations;
    info.totalFrees = totalFrees;
    info.failedAllocations = failedAllocations;

    return &info;
}
