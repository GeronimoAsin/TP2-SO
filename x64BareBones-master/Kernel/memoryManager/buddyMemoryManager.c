#include "memoryManager.h"
#include <stdint.h>
#include <stddef.h>
/*
#define MIN_BLOCK_SIZE 32 
#define MAX_ORDER 20      
#define BLOCK_ALLOCATED 1
#define BLOCK_FREE 0

typedef struct BlockHeader
{
    unsigned int size;       
    unsigned int order;      
    unsigned char allocated; 
} BlockHeader;

typedef struct FreeBlock
{
    struct FreeBlock *next;
    struct FreeBlock *prev;
} FreeBlock;

typedef struct BuddyMemoryManager
{
    uint8_t *heapStart;
    unsigned int heapSize;
    unsigned int minBlockSize;
    unsigned int maxOrder;
    FreeBlock *freeLists[MAX_ORDER + 1];
} BuddyMemoryManager;

static BuddyMemoryManager buddyManager;

MemoryManagerADT createMemoryManager()
{
    uintptr_t alignedStart = ALIGN_POINTER(HEAP_START, WORD_ALIGN);
    buddyManager.heapStart = (uint8_t *)alignedStart;
    buddyManager.heapSize = HEAP_SIZE - (unsigned int)(alignedStart - HEAP_START);
    buddyManager.minBlockSize = MIN_BLOCK_SIZE;

    buddyManager.maxOrder = 0;
    unsigned int blockSize = MIN_BLOCK_SIZE;
    while (blockSize < buddyManager.heapSize && buddyManager.maxOrder < MAX_ORDER)
    {
        blockSize *= 2;
        buddyManager.maxOrder++;
    }

    for (unsigned int i = 0; i <= MAX_ORDER; i++)
    {
        buddyManager.freeLists[i] = NULL;
    }

    if (buddyManager.maxOrder <= MAX_ORDER)
    {
        FreeBlock *initialBlock = (FreeBlock *)buddyManager.heapStart;
        addToFreeList(initialBlock, buddyManager.maxOrder);
    }

    return (MemoryManagerADT)&buddyManager;
}

void *allocateMemory(MemoryManagerADT memoryManager, unsigned int size)
{
    if (memoryManager == NULL || size == 0)
    {
        return NULL;
    }

    unsigned int order = getSizeOrder(size);

    if (order > buddyManager.maxOrder)
    {
        return NULL; 
    }

    void *block = splitBlock(order);

    if (block == NULL)
    {
        return NULL; 
    }

    BlockHeader *header = (BlockHeader *)block;
    header->size = size;
    header->order = order;
    header->allocated = BLOCK_ALLOCATED;

    return (void *)((uintptr_t)block + sizeof(BlockHeader));
}

void freeMemory(MemoryManagerADT memoryManager, void *ptr)
{
    if (memoryManager == NULL || ptr == NULL)
    {
        return;
    }

    BlockHeader *header = (BlockHeader *)((uintptr_t)ptr - sizeof(BlockHeader));
    void *block = (void *)header;

    uintptr_t blockAddr = (uintptr_t)block;
    uintptr_t heapStart = (uintptr_t)buddyManager.heapStart;
    uintptr_t heapEnd = heapStart + buddyManager.heapSize;

    if (blockAddr < heapStart || blockAddr >= heapEnd)
    {
        return;
    }

    if (header->allocated != BLOCK_ALLOCATED)
    {
        return; 
    }

    unsigned int order = header->order;

    header->allocated = BLOCK_FREE;

    while (order < buddyManager.maxOrder)
    {
        void *buddyAddr = getBuddyAddress(block, order);

        if (!isBlockInFreeList(buddyAddr, order))
        {
            break; 
        }

        removeFromFreeList((FreeBlock *)buddyAddr, order);

        if ((uintptr_t)buddyAddr < (uintptr_t)block)
        {
            block = buddyAddr;
        }

        order++;
    }

    addToFreeList((FreeBlock *)block, order);
}

void destroyMemoryManager(MemoryManagerADT memoryManager)
{
    if (memoryManager == NULL)
    {
        return;
    }

    for (unsigned int i = 0; i <= MAX_ORDER; i++)
    {
        buddyManager.freeLists[i] = NULL;
    }

    buddyManager.heapStart = NULL;
    buddyManager.heapSize = 0;
}

static unsigned int getSizeOrder(unsigned int size)
{
    unsigned int adjustedSize = size + sizeof(BlockHeader);
    unsigned int order = 0;
    unsigned int blockSize = MIN_BLOCK_SIZE;

    while (blockSize < adjustedSize && order < MAX_ORDER)
    {
        blockSize *= 2;
        order++;
    }

    return order;
}

static unsigned int getBlockSize(unsigned int order)
{
    return MIN_BLOCK_SIZE << order; 
}

static void *getBuddyAddress(void *block, unsigned int order)
{
    uintptr_t offset = (uintptr_t)block - (uintptr_t)buddyManager.heapStart;
    uintptr_t blockSize = getBlockSize(order);
    uintptr_t buddyOffset = offset ^ blockSize;
    return (void *)((uintptr_t)buddyManager.heapStart + buddyOffset);
}

static void removeFromFreeList(FreeBlock *block, unsigned int order)
{
    if (block->prev)
    {
        block->prev->next = block->next;
    }
    else
    {
        buddyManager.freeLists[order] = block->next;
    }

    if (block->next)
    {
        block->next->prev = block->prev;
    }
}

static void addToFreeList(FreeBlock *block, unsigned int order)
{
    block->next = buddyManager.freeLists[order];
    block->prev = NULL;

    if (buddyManager.freeLists[order])
    {
        buddyManager.freeLists[order]->prev = block;
    }

    buddyManager.freeLists[order] = block;
}

static void *splitBlock(unsigned int order)
{
    if (order > buddyManager.maxOrder)
    {
        return NULL;
    }

    
    if (buddyManager.freeLists[order] == NULL)
    {
        void *largerBlock = splitBlock(order + 1);
        if (largerBlock == NULL)
        {
            return NULL;
        }

        unsigned int blockSize = getBlockSize(order);
        void *buddy = (void *)((uintptr_t)largerBlock + blockSize);

        addToFreeList((FreeBlock *)largerBlock, order);
        addToFreeList((FreeBlock *)buddy, order);
    }

    FreeBlock *block = buddyManager.freeLists[order];
    removeFromFreeList(block, order);

    return (void *)block;
}

static int isBlockInFreeList(void *blockAddr, unsigned int order)
{
    FreeBlock *current = buddyManager.freeLists[order];

    while (current)
    {
        if ((void *)current == blockAddr)
        {
            return 1;
        }
        current = current->next;
    }

    return 0;
}

*/