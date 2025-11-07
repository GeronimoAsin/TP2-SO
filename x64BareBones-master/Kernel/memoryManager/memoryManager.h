#ifndef MEMORY_MANAGER_H
#define MEMORY_MANAGER_H
#include <stddef.h>
#include <stdint.h>
#include "../../Userland/SampleCodeModule/include/memoryInfo.h"

#define MEM_START 0x100000ULL                // dir base de memoria Pure64
#define STACK_SIZE 0x8000ULL                 // 32 KiB de stackBase fijos
#define HEAP_START (ALIGN_POINTER((MEM_START + STACK_SIZE +1), 0x40000ULL))
//#define HEAP_START (MEM_START + STACK_SIZE + 1)
#define HEAP_SIZE 0x400000ULL                // 4 MB de heap

#define CHUNK_SIZE 4096
#define WORD_ALIGN 8
#define CHUNK_COUNT ((unsigned int)(HEAP_SIZE / CHUNK_SIZE))

//Alineamiento
#define ALIGN_POINTER(ptr, alignment) \
    (((uintptr_t)(ptr) + (alignment) - 1) & ~((alignment) - 1))


typedef struct MemoryManagerCDT {
    uint8_t * heapStart;                    // dir base del heap
    unsigned int heapSize;                  // tamaño total heap
    unsigned int chunkSize;                 // tamaño de un solo chunk
    unsigned int chunkCount;                // cantidad de chunks totales
    unsigned int nextFreeIndex;             // Indice del proximo chunk libre
    uint8_t * freeChunkStack[CHUNK_COUNT]; // stack LIFO de chunks libres
} MemoryManagerCDT;

typedef MemoryManagerCDT * MemoryManagerADT;




//crea un memory manager
MemoryManagerADT createMemoryManager();

//retorna la direccion base del segmento asignado, NULL si no hay suficiente memoria
void * allocateMemory(MemoryManagerADT memoryManager, size_t size);


//libera el segmento en la direccion ptr
void freeMemory(MemoryManagerADT memoryManager, void *ptr);

//destruye el memory manager
void destroyMemoryManager(MemoryManagerADT memoryManager);

MemoryInfo *meminfo(void);


#endif // MEMORY_MANAGER_H