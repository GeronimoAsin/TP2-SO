#include "../memoryManager/memoryManager.h"
typedef int pid_t;

typedef struct ProcessQueueCDT * ProcessQueueADT;

ProcessQueueADT createProcessQueue(MemoryManagerADT memoryManager);

void destroyProcessQueue(ProcessQueueADT queue);

void enqueueProcess(ProcessQueueADT queue, pid_t processId);

pid_t dequeueProcess(ProcessQueueADT queue);