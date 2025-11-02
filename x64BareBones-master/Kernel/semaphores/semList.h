#include "../memoryManager/memoryManager.h"
#include "processQueue.h"

typedef char* sem_id;
typedef int sem_t;

typedef struct Semaphore {
    sem_id id;
    sem_t value;
    ProcessQueueADT waitingQueue;
} Semaphore;


typedef struct SemListCDT * SemListADT;

SemListADT createSemList(MemoryManagerADT memoryManager);

void addSemToList(SemListADT list, Semaphore *semaphore);

Semaphore* removeFromSemList(SemListADT list, sem_id semaphoreId);

Semaphore* findInSemList(SemListADT list, sem_id semaphoreId);

int isInSemList(SemListADT list, sem_id semaphoreId);

int semListSize(SemListADT list);

Semaphore* getAtSem(SemListADT list, int index);

void removeFromSemListByProcess(SemListADT list, Semaphore *semaphore);

void clearSemList(SemListADT list);

int isEmptySemList(SemListADT list);

void destroySemList(SemListADT list);

Semaphore* removeFirstFromSemList(SemListADT list);

int isSemListEmpty(SemListADT list);
