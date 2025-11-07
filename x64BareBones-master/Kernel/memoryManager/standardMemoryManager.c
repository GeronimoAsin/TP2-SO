
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
    MemoryManagerADT newMem = &memoryManagerInstance;

    uintptr_t alignedStart = ALIGN_POINTER(HEAP_START, WORD_ALIGN);
    newMem->heapStart = (uint8_t *)alignedStart;

    newMem->heapSize = HEAP_SIZE - (unsigned int)(alignedStart - HEAP_START);
    newMem->chunkSize = CHUNK_SIZE;
    newMem->chunkCount = newMem->heapSize / CHUNK_SIZE;
    newMem->nextFreeIndex = newMem->chunkCount; // Comienza al final del stack

    // Inicializar la pila de chunks libres
    for (unsigned int i = 0; i < newMem->chunkCount; i++)
    {
        newMem->freeChunkStack[i] = newMem->heapStart + (i * newMem->chunkSize);
    }

    return newMem;
}

void * allocateMemory(MemoryManagerADT memoryManager, size_t size) {
    if (memoryManager == NULL || size == 0) {
        // no existe el memory manager o el size es 0
        failedAllocations++;
        return NULL;
    }

    // Si piden más que un chunk, retornar NULL (no podemos satisfacer la solicitud)
    if (size > memoryManager->chunkSize) {
        failedAllocations++;
        return NULL;
    }

    // Verificar si hay chunks disponibles (stack no vacío)
    if (memoryManager->nextFreeIndex == 0) {
        failedAllocations++;
        return NULL; // No hay chunks libres
    }

    // Pop del stack: tomar el chunk en nextFreeIndex - 1
    memoryManager->nextFreeIndex--;
    uint8_t *candidate = memoryManager->freeChunkStack[memoryManager->nextFreeIndex];
    uintptr_t aligned = ALIGN_POINTER(candidate, WORD_ALIGN);

    // Verificar que después del alineamiento el tamaño cabe
    uintptr_t chunkStart = (uintptr_t)candidate;
    uintptr_t chunkEnd = chunkStart + memoryManager->chunkSize;

    if (aligned + size > chunkEnd) {
        // No cabe después del alineamiento, intento con el proximo chunk
        if (memoryManager->nextFreeIndex == 0) {
            // Restauro el índice porque no pudimos usar este chunk
            memoryManager->nextFreeIndex++;
            failedAllocations++;
            return NULL;
        }

        // Devuelvo el chunk al stack y pruebo con el siguiente
        memoryManager->freeChunkStack[memoryManager->nextFreeIndex] = candidate;
        memoryManager->nextFreeIndex--;

        candidate = memoryManager->freeChunkStack[memoryManager->nextFreeIndex];
        aligned = ALIGN_POINTER(candidate, WORD_ALIGN);

        chunkStart = (uintptr_t)candidate;
        chunkEnd = chunkStart + memoryManager->chunkSize;

        if (aligned + size > chunkEnd) {
            // Tampoco cabe en este, devolver ambos al stack
            memoryManager->nextFreeIndex++;
            memoryManager->freeChunkStack[memoryManager->nextFreeIndex] = candidate;
            memoryManager->nextFreeIndex++;
            failedAllocations++;
            return NULL;
        }
    }

    totalAllocations++;

    return (void *)aligned;
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
    // Reiniciar el stack al estado inicial
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





