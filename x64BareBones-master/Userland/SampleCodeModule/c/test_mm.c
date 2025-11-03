#include "syscall.h"
#include "test_util.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "userlib.h"

#define MAX_BLOCKS 128

typedef struct MM_rq {
    void *address;
    uint32_t size;
} mm_rq;

uint64_t test_mm(uint64_t argc, char *argv[]) {

    mm_rq mm_rqs[MAX_BLOCKS];
    uint8_t rq;
    uint32_t total;
    uint64_t max_memory;
    uint64_t iter = 0; // contador de iteraciones para mostrar progreso

    if (argc != 1)
        return -1;

    if ((max_memory = satoi(argv[0])) <= 0)
        return -1;

    printf(" max_memory  read %d",max_memory);

    while (1) {
        rq = 0;
        total = 0;

        // Request as many blocks as we can
        uint32_t alloc_try = 0;
        while (rq < MAX_BLOCKS && total < max_memory) {
            mm_rqs[rq].size = GetUniform(max_memory - total - 1) + 1;
            mm_rqs[rq].address = malloc(mm_rqs[rq].size);

            // DEBUG: durante las primeras 2 iteraciones, registrar las solicitudes

            if (iter < 2) {
                if (mm_rqs[rq].address)
                    // iter y puntero en hexa 64-bit; tamaño como entero firmado (acota bien para este test)
                    printf("test_mm: iter=0x%llx alloc[%d]=%d -> ptr=0x%llx\n",
                           (unsigned long long)iter,
                           (int)alloc_try,
                           (int)mm_rqs[rq].size,
                           (unsigned long long)mm_rqs[rq].address);
                else
                    printf("test_mm: iter=0x%llx alloc[%d]=%d -> NULL\n",
                           (unsigned long long)iter,
                           (int)alloc_try,
                           (int)mm_rqs[rq].size);
            }

            alloc_try++;

            if (mm_rqs[rq].address) {
                total += mm_rqs[rq].size;
                rq++;
            }
        }

        // Set
        uint32_t i;
        for (i = 0; i < rq; i++)
            if (mm_rqs[i].address)
                memset(mm_rqs[i].address, i, mm_rqs[i].size);

        // Check
        for (i = 0; i < rq; i++)
            if (mm_rqs[i].address)
                if (!memcheck(mm_rqs[i].address, i, mm_rqs[i].size)) {
                    printf("test_mm ERROR\n");
                    return -1;
                }

        // Free
        for (i = 0; i < rq; i++)
            if (mm_rqs[i].address)
                free(mm_rqs[i].address);

        // Si no se asignó ningún bloque (malloc falló siempre), evitar busy-loop y esperar
        if (rq == 0) {
            printf("test_mm: malloc all failed for this iter, waiting\n");
            bussy_wait(50000);
            continue;
        }

        iter++;
        if ((iter & 0x3F) == 0) { // cada 64 iteraciones para ver progreso más frecuente
            printf("test_mm: iter=0x%llx last_rq=%d last_total=%d\n",
                   (unsigned long long)iter,
                   (int)rq,
                   (int)total);
        }

    }
}