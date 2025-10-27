#include <stdint.h>
#include <lib.h>
#include <moduleLoader.h>
#include <videoDriver.h>
#include <idtLoader.h>
#include <interrupts.h>
#include "memoryManager/memoryManager.h"
#include "include/lib.h"
#include "processManager/processManager.h"
extern uint8_t text;
extern uint8_t rodata;
extern uint8_t data;
extern uint8_t bss;
extern uint8_t endOfKernelBinary;
extern uint8_t endOfKernel;

static const uint64_t PageSize = 0x1000;

static void *const sampleCodeModuleAddress = (void *)0x400000;
static void *const sampleDataModuleAddress = (void *)0x500000;

typedef int (*EntryPoint)();

extern void beep();

static void printHex64(uint64_t value)
{
    char buf[19]; // "0x" + 16 hex + '\0'
    buf[0] = '0';
    buf[1] = 'x';
    for (int i = 0; i < 16; i++)
    {
        uint8_t nibble = (value >> ((15 - i) * 4)) & 0xF;
        buf[2 + i] = (nibble < 10) ? ('0' + nibble) : ('A' + (nibble - 10));
    }
    buf[18] = '\0';
    printString(buf);
}

// Imprime un uint64_t en formato decimal
static void printDec64(uint64_t value)
{
    char buf[21]; // 20 dígitos + null terminator
    int i = 20;
    buf[i] = '\0';
    if (value == 0)
    {
        buf[--i] = '0';
    }
    else
    {
        while (value > 0 && i > 0)
        {
            buf[--i] = '0' + (value % 10);
            value /= 10;
        }
    }
    printString(&buf[i]);
}

void clearBSS(void *bssAddress, uint64_t bssSize)
{
    memset(bssAddress, 0, bssSize);
}

void *getStackBase()
{
    return (void *)((uint64_t)&endOfKernel + PageSize * 8 // The size of the stack itself, 32KiB
                    - sizeof(uint64_t)                    // Begin at the top of the stack
    );
}

void *initializeKernelBinary()
{
    void *moduleAddresses[] = {
        sampleCodeModuleAddress,
        sampleDataModuleAddress};

    loadModules(&endOfKernelBinary, moduleAddresses);

    clearBSS(&bss, &endOfKernel - &bss);

    return getStackBase();
}

int main()
{
    load_idt();


    // test muy basico del memory manager
    MemoryManagerADT mm = createMemoryManager();
    ProcessManagerADT pm = createProcessManager(mm);

    printString("MM init ");
    newLine();
    if (mm == 0)
    {
        printString("FAIL ");
    }
    else
    {

        // se inicializo bien la memoria
        printString("OK ");
        newLine();

        printString("---------- INFO MEM MANAGER -------");
        newLine();
        printString("heapSize ");
        printDec64(mm->heapSize);
        newLine();

        printString("chunkSize ");
        printDec64(mm->chunkSize);
        newLine();

        printString("chunkCount ");
        printDec64(mm->chunkCount);
        newLine();

        printString("nextFreeIndex ");
        printDec64(mm->nextFreeIndex);
        newLine();

        printString("----------------------------");
        newLine();

        // guardo la dir base de la memoria alocada
        int *p = (int *)allocateMemory(mm, sizeof(int));

        printString("alloc int: ");
        printHex64((uint64_t)p);
        newLine();

        if (p)
        {
            // escribo en la direccion devuelta
            *p = 0x12345678;
            printString("lectura del bloque: ");
            printHex64((uint64_t)(*p));
            newLine();
            // mensaje antes de liberar
            printString("freeing...");
            newLine();
            freeMemory(mm, p);

            printString("freed");
            newLine();
        }

        newLine();
        // Test de chunks consecutivos (similar a memchunks)
        printString("=== Test de chunks consecutivos (kernel) ===");
        newLine();
        void *ptrs[4];
        int i;
        for (i = 0; i < 4; i++)
        {
            ptrs[i] = allocateMemory(mm, 4096);
            printString("allocateMemory(4096) bloque ");
            printDec64(i + 1);
            printString(" = ");
            printHex64((uint64_t)ptrs[i]);
            newLine();
        }
        // Liberar los bloques
        for (int j = 0; j < i; j++)
        {
            freeMemory(mm, ptrs[j]);
        }
        printString("=== Test completado ===");
        newLine();

        // Nota: El siguiente código solo funciona con standardMemoryManager, no con buddyMemoryManager
        printString("heapStart: ");
        printHex64((uint64_t)mm->heapStart);
        newLine();
        printString("Primeros 4 chunks en freeChunkStack:");
        newLine();
        for (int k = 0; k < 4; k++)
        {
            printString("freeChunkStack[");
            printDec64(k);
            printString("]: ");
            printHex64((uint64_t)mm->freeChunkStack[k]);
            newLine();
        }

        newLine();

        /*
        // ============================================
        // Tests del Process Manager
        // ============================================
        printString("=== TESTS DE PROCESS MANAGER ===");
        newLine();

        // Test 1: Verificar que el Process Manager se creó correctamente
        printString("Test 1: Process Manager creado ");
        if (pm != NULL) {
            printString("OK");
        } else {
            printString("FAIL");
        }
        newLine();

        // Test 2: Verificar PID inicial
        printString("Test 2: PID actual (deberia ser -1): ");
        pid_t currentPid = getPid(pm);
        if (currentPid < 0) {
            printString("-");
            printDec64((uint64_t)(-currentPid));
        } else {
            printDec64((uint64_t)currentPid);
        }
        newLine();

        // Test 3: Crear un proceso de prueba simple
        printString("Test 3: Creando proceso de prueba...");
        newLine();

        // Función de prueba simple que no hace nada
        void testFunction(){
            return;
        }

        createProcess(pm, testFunction, 5, "TestProcess", 0, NULL);
        printString("  Proceso de prueba creado");
        newLine();

        // Test 4: Verificar que el maxPid aumentó
        printString("Test 4: maxPid actual: ");
        printDec64((uint64_t)getMaxPid(pm));
        printString(" (deberia ser 1)");
        newLine();

        // Test 5: Verificar que hay procesos en la cola
        printString("Test 5: Verificando cola de procesos...");
        newLine();
        if (getReadyQueue(pm) != 0) {
            printString("  Cola de procesos ready: OK");
        } else {
            printString("  Cola de procesos ready: FAIL");
        }
        newLine();

        // Test 6: Crear el proceso principal (shell)
        printString("Test 6: Creando proceso principal (shell)...");
        newLine();
        createProcess(pm, sampleCodeModuleAddress, 1, "UserlandProcess", 0, NULL);
        printString("  Proceso shell creado");
        newLine();
        printString("  maxPid actual: ");
        printDec64((uint64_t)getMaxPid(pm));
        printString(" (deberia ser 2)");
        newLine();

        // Test 7: Crear otro proceso de prueba para testear modificación de prioridad
        printString("Test 7: Creando proceso para test de prioridad...");
        newLine();
        createProcess(pm, testFunction, 3, "PriorityTest", 0, NULL);
        printString("  Proceso creado con prioridad 3, PID: ");
        printDec64((uint64_t)getMaxPid(pm));
        newLine();

        // Test 8: Modificar prioridad del proceso 3
        printString("Test 8: Modificando prioridad del proceso 3 a 10...");
        newLine();
        modifyPriority(pm, 3, 10);
        printString("  Prioridad modificada");
        newLine();

        // Test 9: Crear proceso para test de bloqueo
        printString("Test 9: Creando proceso para test de bloqueo...");
        newLine();
        createProcess(pm, testFunction, 2, "BlockTest", 0, NULL);
        pid_t blockTestPid = getMaxPid(pm);
        printString("  Proceso creado, PID: ");
        printDec64((uint64_t)blockTestPid);
        newLine();

        // Test 10: Bloquear el proceso
        printString("Test 10: Bloqueando proceso ");
        printDec64((uint64_t)blockTestPid);
        printString("...");
        newLine();
        block(pm, blockTestPid);
        printString("  Proceso bloqueado");
        newLine();

        // Test 11: Desbloquear el proceso
        printString("Test 11: Desbloqueando proceso ");
        printDec64((uint64_t)blockTestPid);
        printString("...");
        newLine();
        unblock(pm, blockTestPid);
        printString("  Proceso desbloqueado");
        newLine();

        // Test 12: Verificar currentProcess
        printString("Test 12: Proceso actual: ");
        if (getCurrentProcess(pm) == NULL) {
            printString("NULL (correcto - ningun proceso ejecutandose)");
        } else {
            printString("PID ");
            printDec64((uint64_t)getCurrentProcess(pm)->pid);
        }
        newLine();

        // Test 13: Verificar total de procesos creados
        printString("Test 13: Total de procesos creados (maxPid): ");
        printDec64((uint64_t)getMaxPid(pm));
        printString(" (deberia ser 4)");
        newLine();

        printString("=== TESTS COMPLETADOS ===");
        newLine();
        newLine();

        // ============================================
        // Limpiar procesos de prueba
        // ============================================
        printString("Limpiando procesos de prueba...");
        newLine();
        clearAllProcesses(pm);
        printString("Procesos de prueba eliminados de la cola");
        newLine();
        newLine();
*/
        // ============================================
        // Crear e iniciar el proceso shell
        // ============================================
        (EntryPoint)sampleCodeModuleAddress;
    return 0;
}

}
