#include "processManager.h"
#include "list.h"
#include "priorityQueue.h"

typedef struct ProcessManagerCDT {
    pid_t currentPid;
    PriorityQueueADT readyQueue;
    ListADT allProcesses;
} ProcessManagerCDT;

