#include "memoryManager.h"
#include <stdint.h>
#include <stddef.h>


static MemoryManagerCDT memoryManagerInstance; 


MemoryManagerADT createMemoryManager()
{
    MemoryManagerADT newMem = &memoryManagerInstance;

    uintptr_t alignedStart = ALIGN_POINTER(HEAP_START, WORD_ALIGN);
    newMem->heapStart = (uint8_t *)alignedStart;

    newMem->heapSize = HEAP_SIZE - (unsigned int)(alignedStart - HEAP_START);
    newMem->chunkSize = CHUNK_SIZE;
    newMem->chunkCount = newMem->heapSize / CHUNK_SIZE;

    newMem->chunkCount = CHUNK_COUNT;
    newMem->nextFreeIndex = 0;
    for (unsigned int i = 0; i < newMem->chunkCount; i++)
    {
        newMem->freeChunkStack[i] = newMem->heapStart + (i * newMem->chunkSize);
    }

    return newMem;


}



void * allocateMemory(MemoryManagerADT memoryManager, unsigned int size) {

    if (memoryManager == NULL || size == 0) {
       //no existe el memory manager o el size es 0
        return NULL;
    }

    if (size > memoryManager->chunkSize) {
        //no hay memoria suficiente
        return NULL;
    }

    if (memoryManager->nextFreeIndex >= memoryManager->chunkCount) {
       //no quedan mas chunks libres
        return NULL;
    }

    uint8_t * candidate = memoryManager->freeChunkStack[memoryManager->nextFreeIndex];
    uintptr_t aligned = ALIGN_POINTER(candidate, WORD_ALIGN);

    //verifico que el tamaño solicitado entra en el chunk
    uintptr_t chunkStart = (uintptr_t)candidate;
    uintptr_t chunkEnd = chunkStart + memoryManager->chunkSize;
    if (aligned + size > chunkEnd) {
        // Se mueve al proximo chunk si no cabe en el actual
        memoryManager->nextFreeIndex++;
        if (memoryManager->nextFreeIndex >= memoryManager->chunkCount) {
            return NULL;
        }
        candidate = memoryManager->freeChunkStack[memoryManager->nextFreeIndex];
        aligned = ALIGN_POINTER(candidate, WORD_ALIGN);

        chunkStart = (uintptr_t)candidate;
        chunkEnd = chunkStart + memoryManager->chunkSize;
        if (aligned + size > chunkEnd) {
            return NULL;
        }
    }

    memoryManager->nextFreeIndex++;
    return (void *)aligned;
}



void freeMemory(MemoryManagerADT memoryManager, void * ptr) {
    if (memoryManager == NULL || ptr == NULL) {
        return;
    }

    uintptr_t start = (uintptr_t)memoryManager->heapStart;
    uintptr_t end = start + (memoryManager->chunkCount * memoryManager->chunkSize);
    uintptr_t p = (uintptr_t)ptr;

    if (p < start || p >= end) {
        // puntero fuera del heap
        return;
    }

    if (memoryManager->nextFreeIndex == 0) {
        //no hay memoria asignada a este chunk
        return;
    }

    //devuelvo el chunk a la pila . No intento calcular el indice exacto del chunk.
    memoryManager->freeChunkStack[--memoryManager->nextFreeIndex] = (uint8_t *)ptr;
}

void destroyMemoryManager(MemoryManagerADT memoryManager) {
    if (memoryManager == NULL) {
        return;
    }

    //Reseteo de indices libres, no es necesario poner la memoria en 0
    memoryManager->nextFreeIndex = 0;
}
