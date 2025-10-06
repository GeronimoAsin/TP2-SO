#ifndef MEMORY_MANAGER_H
#define MEMORY_MANAGER_H
#include <stddef.h> 
#include <stdint.h>

#define MEM_START 0x0000000000100000         // dir base del mapa de memoria
#define MEM_HEAP_SIZE  (16 * 1024 * 1024)   // por ahora dejo 16 MiB porque explota con el tamaño del manual 

#define CHUNK_SIZE 4096
#define WORD_ALIGN 8
#define CHUNK_COUNT ((unsigned int)(MEM_HEAP_SIZE / CHUNK_SIZE)) //cantidad de chunks totales

#define ALIGN_POINTER(ptr, alignment) \
((uintptr_t)(ptr) + ((alignment) - ((uintptr_t)(ptr) % (alignment))))


typedef struct MemoryManagerCDT {
    uint8_t * heapStart;                    // dir base del heap
    unsigned int heapSize;                  // tamaño total heap
    unsigned int chunkSize;                 // tamaño de un solo chunk
    unsigned int chunkCount;                // cantidad de chunks totales
    unsigned int nextFreeIndex;             // Indice del proximo chunk libre
    uint8_t * freeChunkStack[CHUNK_COUNT];  // stack LIFO de chunks libres
} MemoryManagerCDT;

typedef MemoryManagerCDT * MemoryManagerADT; 




//crea un memory manager
MemoryManagerADT createMemoryManager();

//retorna la direccion base del segmento asignado, NULL si no hay suficiente memoria
void * allocateMemory(MemoryManagerADT memoryManager, unsigned int size);


//libera el segmento en la direccion ptr
void freeMemory(MemoryManagerADT memoryManager, void *ptr);

//destruye el memory manager
void destroyMemoryManager(MemoryManagerADT memoryManager);

#endif // MEMORY_MANAGER_H