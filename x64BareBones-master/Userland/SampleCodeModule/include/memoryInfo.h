#ifndef MEMORY_INFO_H
#define MEMORY_INFO_H

#include <stdint.h>

typedef struct MemoryInfo {
    uint64_t heapStart;
    uint64_t heapSize;
    uint64_t chunkSize;
    uint64_t chunkCount;
    uint64_t usedBytes;
    uint64_t freeBytes;
    uint64_t totalAllocations;
    uint64_t totalFrees;
    uint64_t failedAllocations;
} MemoryInfo;

#endif /* MEMORY_INFO_H */
