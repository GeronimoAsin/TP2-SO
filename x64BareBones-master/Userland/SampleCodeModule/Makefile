include ../Makefile.inc

MODULE=0000-sampleCodeModule.bin
SOURCES=$(wildcard [^_]*.c) c/userlib.c	c/shell.c c/getTime.c c/printRegisters.c
ASM_SOURCES=asm/libasm.asm
ASM_OBJECTS=$(ASM_SOURCES:.asm=.o)

all: $(MODULE)

$(MODULE): $(SOURCES) $(ASM_OBJECTS)
	$(GCC) $(GCCFLAGS) -T sampleCodeModule.ld _loader.c $(SOURCES) $(ASM_OBJECTS) -o ../$(MODULE)

$(ASM_OBJECTS): $(ASM_SOURCES)
	$(ASM) $(ASMFLAGS) $< -o $@

clean:
	rm -rf *.o asm/*.o

.PHONY: all clean print
