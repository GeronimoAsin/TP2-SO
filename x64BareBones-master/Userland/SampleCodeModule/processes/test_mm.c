#include "syscall.h"
#include "test_util.h"
#include "userlib.h"

#define MAX_BLOCKS 5

typedef struct MM_rq {
  void *address;
  uint32_t size;
} mm_rq;

uint64_t test_mm(uint64_t argc, char *argv[]) {

  printf("Iniciando Test del Memory Manager\n");
  mm_rq mm_rqs[MAX_BLOCKS];
  uint8_t rq;
  uint32_t total;
  uint64_t max_memory;

  if (argc != 1)
    my_exit();

  if ((max_memory = satoi(argv[0])) <= 0)
    my_exit();

  while (1) {
    rq = 0;
    total = 0;

	printf("entrando al while\n");
    // Request as many blocks as we can
    while (rq < MAX_BLOCKS && total < max_memory) {
	printf("entramos\n");
      mm_rqs[rq].size = GetUniform(max_memory - total - 1) + 1;
      printf("Solicitando size: %d\n", mm_rqs[rq].size);
      mm_rqs[rq].address = malloc(mm_rqs[rq].size);

      if (mm_rqs[rq].address) {
        printf("Malloc exitoso, address: 0x%x\n", (uint64_t)mm_rqs[rq].address);
        total += mm_rqs[rq].size;
        rq++;
      } else {
        printf("Malloc fallo\n");
      }
    }

	printf("entrando a set (rq=%d)\n", rq);
    // Set
    uint32_t i;
    for (i = 0; i < rq; i++) {
	 printf("entrando al if \n");
      if (mm_rqs[i].address) {
        printf("Seteando bloque %d, size: %d, addr: 0x%x\n", i, mm_rqs[i].size, (uint64_t)mm_rqs[i].address);
        memset(mm_rqs[i].address, i, mm_rqs[i].size);
        printf("Memset completado para bloque %d\n", i);
		if(i==rq-1)
		{
			printf("for completado\n");
		}
      }
    }

	printf("entrando a check\n");
    // Check
    for (i = 0; i < rq; i++)
      if (mm_rqs[i].address)
        if (!memcheck(mm_rqs[i].address, i, mm_rqs[i].size)) {
          printf("test_mm ERROR\n");
          my_exit();
        }

	printf("entrando a free\n");
    // Free
    for (i = 0; i < rq; i++)
      if (mm_rqs[i].address)
        free(mm_rqs[i].address);

	printf("Exito parcial\n");
  }
  printf("Test del Memory Manager superado\n");
  my_exit();
}