#ifndef SEMAPHORES_H
#define SEMAPHORES_H

extern void acquire();
extern void release();
typedef char* sem_id;
typedef int sem_t;
typedef struct SemaphoresCDT * SemaphoresADT;
typedef struct MemoryManagerCDT * MemoryManagerADT;
typedef struct ProcessManagerCDT * ProcessManagerADT;

SemaphoresADT semaphores_create(MemoryManagerADT memoryManager, ProcessManagerADT processManager);

SemaphoresADT getGlobalSemaphoresManager();

void semaphores_destroy(SemaphoresADT semaphores);

void sem_open(SemaphoresADT semaphores, sem_id sem, sem_t initialValue);

void sem_post(SemaphoresADT semaphores, sem_id sem);

void sem_wait(SemaphoresADT semaphores, sem_id sem);

void sem_close(SemaphoresADT semaphores, sem_id sem);

#endif // SEMAPHORES_H