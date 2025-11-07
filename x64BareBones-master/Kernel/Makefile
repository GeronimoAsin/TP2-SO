include Makefile.inc

KERNEL=kernel.bin

# Detectar si se está usando el target buddy
ifeq ($(MAKECMDGOALS),buddy)
    MM_OBJ=memoryManager/buddyMemoryManager.o
    MM_EXCLUDE_OBJ=memoryManager/standardMemoryManager.o
    MM_SRC=memoryManager/buddyMemoryManager.c
else
    MM_OBJ=memoryManager/standardMemoryManager.o
    MM_EXCLUDE_OBJ=memoryManager/buddyMemoryManager.o
    MM_SRC=memoryManager/standardMemoryManager.c
endif

# Archivos fuente (excluyendo memory managers - se manejan por separado)
SOURCES_TEMP=$(wildcard *.c */*.c)
SOURCES=$(filter-out memoryManager/buddyMemoryManager.c memoryManager/standardMemoryManager.c,$(SOURCES_TEMP))

SOURCES_ASM=$(wildcard asm/*.asm interrupt/*.asm  processManager/*.asm semaphores/*.asm)
OBJECTS=$(SOURCES:.c=.o)
OBJECTS_ASM=$(SOURCES_ASM:.asm=.o)
LOADERSRC=loader.asm

LOADEROBJECT=$(LOADERSRC:.asm=.o)
STATICLIBS=

all: clean_mm $(KERNEL)

buddy: clean_mm $(KERNEL)
	@echo "Kernel compilado con Buddy Memory Manager"

# Limpiar AMBOS memory managers antes de compilar
clean_mm:
	@rm -f memoryManager/*.o

# Regla específica para compilar el memory manager seleccionado
$(MM_OBJ): $(MM_SRC)
	$(GCC) $(GCCFLAGS) -I./include -c $< -o $@

$(KERNEL): $(LOADEROBJECT) $(OBJECTS) $(MM_OBJ) $(STATICLIBS) $(OBJECTS_ASM)
	$(LD) $(LDFLAGS) -T kernel.ld -o $(KERNEL) $(LOADEROBJECT) $(OBJECTS) $(MM_OBJ) $(OBJECTS_ASM) $(STATICLIBS)

%.o: %.c
	$(GCC) $(GCCFLAGS) -I./include -c $< -o $@

%.o : %.asm
	$(ASM) $(ASMFLAGS) $< -o $@

$(LOADEROBJECT):
	$(ASM) $(ASMFLAGS) $(LOADERSRC) -o $(LOADEROBJECT)

clean:
	rm -rf asm/*.o interrupt/*.o processManager/*.o semaphores/*.o memoryManager/*.o pipes/*.o *.o *.bin

.PHONY: all clean buddy clean_mm
