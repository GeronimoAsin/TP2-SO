// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include "semaphores.h"
#include "semList.h"
#include "processQueue.h"
#include "../processManager/processManager.h"


typedef struct SemaphoresCDT {
    SemListADT openedSemaphores;
    MemoryManagerADT memoryManager;
    ProcessManagerADT processManager;
} SemaphoresCDT;

static SemaphoresADT globalSemaphores = NULL;

SemaphoresADT getGlobalSemaphoresManager() {
    return globalSemaphores;
}

SemaphoresADT semaphores_create(MemoryManagerADT memoryManager, ProcessManagerADT processManager) {
    SemaphoresADT semaphores = (SemaphoresADT)allocateMemory(memoryManager, sizeof(SemaphoresCDT));
    semaphores->openedSemaphores = createSemList(memoryManager);
    semaphores->memoryManager = memoryManager;
    semaphores->processManager = processManager;
    globalSemaphores = semaphores;
    return semaphores;
}

void semaphores_destroy(SemaphoresADT semaphores) {
    clearSemList(semaphores->openedSemaphores);
    freeMemory(semaphores->memoryManager, semaphores);
}

void sem_open(SemaphoresADT semaphores, sem_id sem, sem_t initialValue) {
    if(isInSemList(semaphores->openedSemaphores, sem)) {
        return; 
    }
    Semaphore *newSemaphore = (Semaphore *)allocateMemory(semaphores->memoryManager, sizeof(Semaphore));
    newSemaphore->id = sem;
    newSemaphore->value = initialValue;
    newSemaphore->waitingQueue = createProcessQueue(semaphores->memoryManager);
    addSemToList(semaphores->openedSemaphores, newSemaphore);
}

void sem_close(SemaphoresADT semaphores, sem_id sem) {
    if (!isInSemList(semaphores->openedSemaphores, sem)) {
        return;
    }
    Semaphore *semToRemove = removeFromSemList(semaphores->openedSemaphores, sem);
    destroyProcessQueue(semToRemove->waitingQueue);
    freeMemory(semaphores->memoryManager, semToRemove);
}

void sem_post(SemaphoresADT semaphores, sem_id sem) {
    Semaphore *semaphore = findInSemList(semaphores->openedSemaphores, sem);
    if (semaphore == NULL) {
        return; 
    }
    acquire();
    if(semaphore->value == 0) {
        pid_t nextProcess = dequeueProcess(semaphore->waitingQueue);
        if(nextProcess != -1) {
            unblock(semaphores->processManager, nextProcess);
        } else {
            semaphore->value++;
        }
        release();
    }else {
        semaphore->value++;
        release();
    }
}

void sem_wait(SemaphoresADT semaphores, sem_id sem) {
    Semaphore *semaphore = findInSemList(semaphores->openedSemaphores, sem);
    if (semaphore == NULL) {
        return; 
    }
    acquire();
    if(semaphore->value > 0) {
        semaphore->value--;
        release();
    } else {
        pid_t currentProcess = getPid(semaphores->processManager);
        enqueueProcess(semaphore->waitingQueue, currentProcess);
        release();
        block(semaphores->processManager, currentProcess);
    }
}