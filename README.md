# TP 2 - SO - Grupo 10
## Construcción del Núcleo del Sistema Operativo y mecanismos de administración de recursos

### Instrucciones de compilación y ejecución
Para poder compilar y ejecutar el trabajo práctico se deben modificar los datos correspondientes al contenedor de docker en los archivos **compile.sh** y **run.sh**. Luego, simplemente se debe ejecutar el primero para compilar y el segundo para ejecutar.  
Por defecto se emplea el Standard Memory Manager En caso de desear usar el Buddy Memory Manager se debe ejecutar **compile.sh buddy**.  

### Instrucciones de replicación
#### Comandos disponibles
**clear**: Borra el contenido de la pantalla. No admite parámetros.  
**echo <texto>**: Imprime el texto en salida estándar. El único parámetro admitido es dicho texto.  
**time**: Imprime en salida estándar la hora UTC y la hora Argentina. No admite parámetros.  
**help**: Imprime un mensaje de ayuda que explica qué comandos están disponibles y cómo usarlos. No admite parámetros.  
**registers**: Imprime el estado de los registros del CPU la última vez que se oprimió la tecla ESC. No admite parámetros.  
**mem**: Imprime el estado de la memoria en salida estándar. No admite parámetros.  
**loop <secs>**: Loop infinito que imprime un saludo cada la cantidad de segundos indicada. El único parámetro admitido es la cantidad de segundos (opcional).  
**fg**: Trae a foreground el primer proceso que haya ejecutando en background. No admite parámetros.  
**kill <pid>**: Mata el proceso cuyo identificador se indica. El único parámetro que admite el identificador del proceso.  
**nice <pid> <priority>**: Cambia la prioridad del proceso que se indica. Recibe como parámetros el identificador del proceso y la nueva prioridad (se espera un unsigned int).  
**block <pid>**: Bloquea el proceso cuyo identificador se pasa como parámetro. Es el único parámetro admitido.  
**unblock <pid>**: Desbloquea el proceso cuyo identificador se pasa como parámetro. Es el único parámetro admitido.  
**cat**: Imprime lo que recibe por entrada estándar. No admite parámetros.  
**wc**: Imprime la cantidad de líneas de lo que recibe por entrada estándar. No admite parámetros.  
**filter**: Filtra las vocales de lo que recibe por entrada estándar. No admite parámetros.  
**mvar <escritores> <lectores>**: Implementa un problema de lectores y escritores similar a MVar de Haskell. Recibe como parámetros la cantidad de escritores y de lectores.  
**test_mm <max_memory>**:Ejecuta un ciclo infinito que pide y libera bloques de tamaño aleatorio chequeando en cada implementación que no se solapen. Recibe como parámetro la cantidad de memoria máxima a utilizar en bytes.  
**test_processes <num_processes>**: Ciclo infinito que crea, bloquea, desbloquea y mata procesos dummy aleatoriamente. Toma como parámetros la cantidad máxima de procesos a crear.  
**test_prio <final_value>**: Crea tres procesos que incrementan, cada uno, una variable inicializada en 0. En primera instancia los 3 procesos tienen la misma prioridad y luego se vuelven a ejecutar con prioridades distintas. Toma como parámetro el valor al que deben llegar las variables para finalizar.  
**test_synchro <num_processes> <synchro>**: Crea la cantidad de procesos indicada y los mismos incrementan y decrementan una misma variable global a fin de visualizar los efectos del uso de semáforo. Toma como parámetros la cantidad de procesos a crear y un valor que indica si debe (1) o no (0) usar semáforos.  

#### Caractéres especiales
**|**: permite pipear dos comandos.  
**&**: permite ejecutar un proceso en background.

#### Atajos de teclado
**Ctrl + C**: interrumpe la ejecución de un programa.  
**Ctrl + D**: envia EOF.

#### Ejemplos para demostrar el funcionamiento de cada requerimiento
**Procesos, Context Switching y Scheduling**: Se pueden crear varios procesos (generealmente usando loop), mandarlos a background y "jugar" con los comandos disponibles.  
**Sincronización**: La mejor prueba de su funcionamiento es el comando mvar. Adeás, la forma en que hicimos que la syscall read fuera bloqueante por ejemplo fue a través de un semáforo.  
**Inter Process Comunication**: Se pueden pipear dos procesos con comandos como cat, filter o wc. Por ejemplo, **help | wc** o **echo Hola Mundo | filter**.

#### Limitaciones
Durante las distintas pruebas realizadas, se notó que puede generarse deadlock en mvar en caso de que se mate a un proceso que debiera liberar un semáforo. Al no usar señales en el trabajo práctico no se nos ocurrió una solución.  
Por otra parte, hicimos que test_processes saliera luego de haber creado y matado todos los procesos indicados, al no entender la finalidad de un ciclo infinito. De todas formas, simplemente debería sacarse el my_exit() de la última linea para que vuelva a ser un ciclo infinito.  

#### Uso de IA
El uso de herramientas de Inteligencia Artificial se limitó a funcionalidades ajenas a la materia (como por ejemplo manejo de Strings o la funcionalidad para hacer que el Makefile compile archivos distintos al agregar buddy) cuya implementación hubiera sido una pérdida de tiempo y tareas de debugging. Sin embargo, esto no implica desconocimiento por parte de ninguno de los integrantes del grupo de la implementación entregada.

#### Sobre el análisis con PVS-Studio
Obtuvimos algunos mensajes que consideramos irrelevantes:  
- Aquellos relacionados con archivos de Bootloader, Toolchain, los tests desarrollados por la cátedra, y la línea 6 de sampelCodeModule.c por no corresponder a código de nuestra autoría ni para esta materia ni para Arquitectura de Computadoras.  
- kernel.c líneas 23 y 24: The integer constant is converted to pointer. Possibly an error or a bad coding style: (void *) 0x400000. No sabemos de que otra forma indicar la dirección de memoria. Vale la pena mencionar, también corresponde a algo propio de Pure64 y su desarrollo.
- standardMemoryManager.c 24: The integer constant is converted to pointer. Possibly an error or a bad coding style. No sabemos de que otra forma indicar la dirección de memoria.  
- buddyMemoryManager.c 187: The integer constant is converted to pointer. Possibly an error or a bad coding style. No sabemos de que otra forma indicar la dirección de memoria.  
- shell.c línea 42: A part of conditional expression is always true: c <= 126. Es una validación que está ya hecha en el kernel, pero consideramos que no está de más.
- kill.c líneas 15 y 17: The 'argv' pointer was used unsafely after it was verified against nullptr. No vemos por qué seria accedido de manera insegura, ya que se realizan validaciones previamente.  
- loop.c línea 35: Potentially infinite loop. Es la idea, generar un loop infinito.
- mvar.c líneas 48 y 88: Potentially infinite loop. Es la idea, generar un loop infinito.  
- nice.c líneas 15,19 y 24: The 'argv' pointer was used unsafely after it was verified against nullptr. No vemos por qué sería accedido de manera insegura, ya que se realizan validaciones previamente.  
