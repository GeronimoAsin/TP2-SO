#include <stdint.h>
#include <string.h>
#include <lib.h>
#include <moduleLoader.h>
#include <videoDriver.h>
#include <idtLoader.h>
#include "memoryManager/memoryManager.h"
#include "include/lib.h"
extern uint8_t text;
extern uint8_t rodata;
extern uint8_t data;
extern uint8_t bss;
extern uint8_t endOfKernelBinary;
extern uint8_t endOfKernel;

static const uint64_t PageSize = 0x1000;

static void * const sampleCodeModuleAddress = (void*)0x400000;
static void * const sampleDataModuleAddress = (void*)0x500000;

typedef int (*EntryPoint)();

extern void beep();

static void printHex64(uint64_t value)
{
    char buf[19]; // "0x" + 16 hex + '\0'
    buf[0] = '0';
    buf[1] = 'x';
    for (int i = 0; i < 16; i++) {
        uint8_t nibble = (value >> ((15 - i) * 4)) & 0xF;
        buf[2 + i] = (nibble < 10) ? ('0' + nibble) : ('A' + (nibble - 10));
    }
    buf[18] = '\0';
    printString(buf);
}

void clearBSS(void * bssAddress, uint64_t bssSize)
{
	memset(bssAddress, 0, bssSize);
}

void * getStackBase()
{
	return (void*)(
		(uint64_t)&endOfKernel
		+ PageSize * 8				//The size of the stack itself, 32KiB
		- sizeof(uint64_t)			//Begin at the top of the stack
	);
}

void * initializeKernelBinary()
{
	void * moduleAddresses[] = {
		sampleCodeModuleAddress,
		sampleDataModuleAddress
	};

	loadModules(&endOfKernelBinary, moduleAddresses);

	clearBSS(&bss, &endOfKernel - &bss);

	return getStackBase();
}

int main()
{
	load_idt();

	//test muy basico del memory manager
    MemoryManagerADT mm = createMemoryManager();
    printString("MM init ");
	newLine();
    if (mm == 0) {
        printString("FAIL ");
    } else {

		//se inicializo bien la memoria
        printString("OK ");
		newLine();

		//guardo la dir base de la memoria alocada
        int *p = (int *)allocateMemory(mm, sizeof(int));

        printString("alloc int: "); printHex64((uint64_t)p); newLine();
        if (p) {
			//escribo en la direccion devuelta
            *p = 0x12345678;
            printString("lectura del bloque: "); printHex64((uint64_t)(*p)); printString("hola");newLine(); //aca esta fallando 
            // mensaje antes de liberar
            printString("freeing..."); newLine();
            freeMemory(mm, p);
            printString("freed"); newLine();
			//se libero correctamente la memoria
        }
    }
	//entry point a Userland 
	//((EntryPoint)sampleCodeModuleAddress)();

	while(1);
}



