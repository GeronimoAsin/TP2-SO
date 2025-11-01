#include "memoryManager.h"
#include "../include/lib.h"
#include <stdint.h>
#include <stddef.h>

#define MIN_BLOCK_SIZE 32
#define MAX_ORDER 12                // 2^12 = 4096 bytes
#define BUDDY_MAGIC 0xBDD7BA8E      // magic number para que validemos si hay corrupcion de memoria


typedef struct BuddyBlockHeader {
    uint32_t magic;                 // magic number para validación que no se corrompio la mem
    uint8_t order;                  // Orden del bloque
    uint8_t is_free;                // 1 = libre 0 = ocupado
    uint16_t padding;               // alineación
    struct BuddyBlockHeader* next;  // Next lista libre
    struct BuddyBlockHeader* prev;  // Prev lista libre
} BuddyBlockHeader;


typedef struct BuddyMemoryManagerCDT {
    uint8_t* heapStart;                         // Dirección base heap
    unsigned int heapSize;                      // Tamaño total heap
    unsigned int chunkSize;
    unsigned int chunkCount;                    // estos campos por las dudas los dejo
    unsigned int nextFreeIndex;
    uint8_t* freeChunkStack[CHUNK_COUNT];

    BuddyBlockHeader* free_lists[MAX_ORDER + 1]; // Listas de bloques libres por orden
    uint8_t max_order;                          // Orden máximo disponible

    // Estadísticas
    size_t allocated_bytes;                     // Bytes asignados
    size_t total_allocations;                   // Contador de asignaciones
    size_t total_frees;                         // Contador de liberaciones
    size_t failed_allocations;                  // Asignaciones fallidas
} BuddyMemoryManagerCDT;

// Instancia global del memory manager
static BuddyMemoryManagerCDT buddyMemoryManagerInstance;


static uint8_t buddy_get_order(size_t size) {
    // Agregar espacio para el header
    size_t total_size = size + sizeof(BuddyBlockHeader);

    // Calcular orden mínimo necesario
    if (total_size <= MIN_BLOCK_SIZE) {
        return 0;
    }

    uint8_t order = 0;
    size_t block_size = MIN_BLOCK_SIZE;

    while (block_size < total_size && order < MAX_ORDER) {
        block_size <<= 1;  // block_size *= 2
        order++;
    }

    return order;
}


 // Convierte un orden a tamaño en bytes
 // order: Orden del bloque
 // return: Tamaño en bytes
static size_t buddy_order_to_size(uint8_t order) {
    return MIN_BLOCK_SIZE << order;  // MIN_BLOCK_SIZE * 2^order
}


 // Calcula la dirección del bloque buddy
 // mm: Memory manager
 // block: Bloque actual
 // order: Orden del bloque
 // return: Puntero al bloque buddy o NULL si no existe
static void* buddy_get_buddy(BuddyMemoryManagerCDT* mm, void* block, uint8_t order) {
    uintptr_t block_addr = (uintptr_t)block;
    uintptr_t heap_start = (uintptr_t)mm->heapStart;

    // Calcular offset relativo al inicio del heap
    uintptr_t offset = block_addr - heap_start;

    // El buddy se obtiene haciendo XOR con el tamaño del bloque
    size_t block_size = buddy_order_to_size(order);
    uintptr_t buddy_offset = offset ^ block_size;

    // Verificar que el buddy está dentro del heap
    if (buddy_offset + block_size > mm->heapSize) {
        return NULL;
    }

    return (void*)(heap_start + buddy_offset);
}


// Verifica si un puntero está dentro del rango válido del heap
// mm: Memory manager
// ptr: Puntero a verificar
// return: 1 si es válido, 0 si no
static int buddy_is_valid_pointer(BuddyMemoryManagerCDT* mm, void* ptr) {
    if (!ptr || !mm) {
        return 0;
    }

    uintptr_t addr = (uintptr_t)ptr;
    uintptr_t start = (uintptr_t)mm->heapStart;
    uintptr_t end = start + mm->heapSize;

    return (addr >= start && addr < end);
}

/**
 Elimina un bloque de su lista libre
 mm: Memory manager
 block: Bloque a eliminar
 order: Orden del bloque
*/
static void buddy_remove_from_free_list(BuddyMemoryManagerCDT* mm, BuddyBlockHeader* block, uint8_t order) {
    if (!block || order > MAX_ORDER) {
        return;
    }

    // Actualizar enlaces
    if (block->prev) {
        block->prev->next = block->next;
    } else {
        // Es el primero de la lista
        mm->free_lists[order] = block->next;
    }

    if (block->next) {
        block->next->prev = block->prev;
    }

    block->next = NULL;
    block->prev = NULL;
}


 // Agrega un bloque a su lista libre
 // mm: Memory manager
 // block: Bloque a agregar
 // order: Orden del bloque
static void buddy_add_to_free_list(BuddyMemoryManagerCDT* mm, BuddyBlockHeader* block, uint8_t order) {
    if (!block || order > MAX_ORDER) {
        return;
    }

    block->order = order;
    block->is_free = 1;
    block->magic = BUDDY_MAGIC;

    // Insertar al inicio de la lista
    block->next = mm->free_lists[order];
    block->prev = NULL;

    if (mm->free_lists[order]) {
        mm->free_lists[order]->prev = block;
    }

    mm->free_lists[order] = block;
}

//Fusiona un bloque con su buddy si está libre
//block: Bloque a fusionar
//return: Puntero al bloque fusionado (puede variar al original)
static BuddyBlockHeader* buddy_coalesce(BuddyMemoryManagerCDT* mm, BuddyBlockHeader* block) {
    if (!block || block->order >= mm->max_order) {
        return block;
    }

    // Busco el bloque buddy
    void* buddy_addr = buddy_get_buddy(mm, block, block->order);

    if (!buddy_addr || !buddy_is_valid_pointer(mm, buddy_addr)) {
        return block;
    }

    BuddyBlockHeader* buddy = (BuddyBlockHeader*)buddy_addr;

    // Verifico que el buddy es válido y está libre
    if (buddy->magic != BUDDY_MAGIC || !buddy->is_free || buddy->order != block->order) {
        return block;
    }

    // Determinar cuál bloque es el de menor dirección (será el bloque fusionado)
    BuddyBlockHeader* lower = (block < buddy) ? block : buddy;
    BuddyBlockHeader* higher = (block < buddy) ? buddy : block;

    // Removemos ambos bloques de sus listas
    buddy_remove_from_free_list(mm, lower, lower->order);
    buddy_remove_from_free_list(mm, higher, higher->order);

    // Creamos el bloque fusionado
    uint8_t new_order = lower->order + 1;
    buddy_add_to_free_list(mm, lower, new_order);

    // fusionamos recursivamente
    return buddy_coalesce(mm, lower);
}



MemoryManagerADT createMemoryManager() {
    BuddyMemoryManagerCDT* mm = &buddyMemoryManagerInstance;

    //inicializo los campos
    uintptr_t alignedStart = ALIGN_POINTER(HEAP_START, WORD_ALIGN);
    mm->heapStart = (uint8_t*)alignedStart;
    mm->heapSize = HEAP_SIZE - (unsigned int)(alignedStart - HEAP_START);
    mm->chunkSize = CHUNK_SIZE;
    mm->chunkCount = mm->heapSize / CHUNK_SIZE;
    mm->nextFreeIndex = 0;

    // Inicializacion listas libres
    for (int i = 0; i <= MAX_ORDER; i++) {
        mm->free_lists[i] = NULL;
    }

    // Calcular orden máximo basado en el tamaño del heap
    mm->max_order = 0;
    size_t test_size = MIN_BLOCK_SIZE;
    while (test_size <= mm->heapSize && mm->max_order < MAX_ORDER) {
        test_size <<= 1;
        mm->max_order++;
    }
    mm->max_order--;  // Ajuste al último orden válido

    // estadisticas
    mm->allocated_bytes = 0;
    mm->total_allocations = 0;
    mm->total_frees = 0;
    mm->failed_allocations = 0;

    // Bloque inicial que abarca el heap
    size_t offset = 0;
    while (offset < mm->heapSize) {
        size_t remaining = mm->heapSize - offset;

        // Encontrar el orden más grande que cabe
        uint8_t order = 0;
        size_t block_size = MIN_BLOCK_SIZE;

        while (block_size <= remaining && order < mm->max_order) {
            block_size <<= 1;
            order++;
        }

        // Usar el orden anterior si el actual es muy grande
        if (block_size > remaining) {
            order--;
            block_size >>= 1;
        }

        // Creacion del bloque en esta posición
        if (block_size >= MIN_BLOCK_SIZE) {
            BuddyBlockHeader* block = (BuddyBlockHeader*)(mm->heapStart + offset);
            buddy_add_to_free_list(mm, block, order);
            offset += block_size;
        } else {
            break;  // No cabe más
        }
    }

    return (MemoryManagerADT)mm;
}


void* allocateMemory(MemoryManagerADT memoryManager, size_t size) {
    BuddyMemoryManagerCDT* mm = (BuddyMemoryManagerCDT*)memoryManager;

    // Validaciones básicas
    if (!mm || size == 0) {
        if (mm) mm->failed_allocations++;
        return NULL;
    }

    // Calcular orden necesario
    uint8_t order = buddy_get_order(size);

    if (order > mm->max_order) {
        mm->failed_allocations++;
        return NULL;
    }

    // Buscar bloque libre del orden adecuado
    BuddyBlockHeader* block = NULL;

    // Primero intentar encontrar un bloque del orden exacto
    for (uint8_t o = order; o <= mm->max_order; o++) {
        if (mm->free_lists[o]) {
            block = mm->free_lists[o];
            buddy_remove_from_free_list(mm, block, o);

            // Si es más grande del necesario, dividir
            while (block->order > order) {
                uint8_t current_order = block->order;
                size_t half_size = buddy_order_to_size(current_order - 1);

                // Crear el buddy superior
                BuddyBlockHeader* upper_buddy = (BuddyBlockHeader*)((uint8_t*)block + half_size);
                buddy_add_to_free_list(mm, upper_buddy, current_order - 1);

                // Actualizar el bloque actual
                block->order = current_order - 1;
            }

            break;
        }
    }

    // Si no encontramos bloque, falló la asignación
    if (!block) {
        mm->failed_allocations++;
        return NULL;
    }

    // Marcar bloque como ocupado
    block->is_free = 0;
    block->magic = BUDDY_MAGIC;
    block->order = order;

    // Actualizar estadísticas
    mm->allocated_bytes += buddy_order_to_size(order);
    mm->total_allocations++;

    // Retornar puntero después del header
    return (void*)((uint8_t*)block + sizeof(BuddyBlockHeader));
}


void freeMemory(MemoryManagerADT memoryManager, void* ptr) {
    BuddyMemoryManagerCDT* mm = (BuddyMemoryManagerCDT*)memoryManager;

    // Validaciones
    if (!mm || !ptr) {
        return;
    }

    // Obtener el header del bloque
    BuddyBlockHeader* block = (BuddyBlockHeader*)((uint8_t*)ptr - sizeof(BuddyBlockHeader));

    // Validar que es un bloque válido
    if (!buddy_is_valid_pointer(mm, block)) {
        return;
    }

    if (block->magic != BUDDY_MAGIC) {
        return;  // Bloque corrupto o inválido
    }

    if (block->is_free) {
        return;  // Double-free detectado
    }

    if (block->order > mm->max_order) {
        return;  // Orden inválido
    }

    // Actualizar estadísticas
    mm->allocated_bytes -= buddy_order_to_size(block->order);
    mm->total_frees++;

    // Marcar como libre
    block->is_free = 1;

    // Agregar a la lista libre
    buddy_add_to_free_list(mm, block, block->order);

    // Intentar fusionar con buddies adyacentes
    buddy_coalesce(mm, block);
}


void destroyMemoryManager(MemoryManagerADT memoryManager) {
    BuddyMemoryManagerCDT* mm = (BuddyMemoryManagerCDT*)memoryManager;

    if (!mm) {
        return;
    }

    // Limpiar todas las listas libres
    for (int i = 0; i <= MAX_ORDER; i++) {
        mm->free_lists[i] = NULL;
    }

    // Reinicializar estadísticas
    mm->allocated_bytes = 0;
    mm->total_allocations = 0;
    mm->total_frees = 0;
    mm->failed_allocations = 0;

    // Recrear el bloque inicial
    size_t offset = 0;
    while (offset < mm->heapSize) {
        size_t remaining = mm->heapSize - offset;

        uint8_t order = 0;
        size_t block_size = MIN_BLOCK_SIZE;

        while (block_size <= remaining && order < mm->max_order) {
            block_size <<= 1;
            order++;
        }

        if (block_size > remaining) {
            order--;
            block_size >>= 1;
        }

        if (block_size >= MIN_BLOCK_SIZE) {
            BuddyBlockHeader* block = (BuddyBlockHeader*)(mm->heapStart + offset);
            buddy_add_to_free_list(mm, block, order);
            offset += block_size;
        } else {
            break;
        }
    }
}


MemoryManagerADT meminfo() {
    return (MemoryManagerADT)&buddyMemoryManagerInstance;
}

