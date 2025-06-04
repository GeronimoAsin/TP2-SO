#include "include/keyboardDriver.h"
#include <stdint.h>
#include "include/videoDriver.h"
#include "include/naiveConsole.h"

#define KEYBOARD_BUFFER_SIZE 128

//buffer circular de teclado
//guardo 2 indices para saber donde guardar y donde leer el proximo caracter
static char keyboard_buffer[KEYBOARD_BUFFER_SIZE];
static int kb_head = 0, kb_tail = 0;
static int shift = 0;


extern void read(uint8_t *data); // función asm


static const char scancode_to_ascii[128] = {
    0, 27, '1', '2', '3', '4', '5', '6', '7', '8', '9', '0', '-', '=', '\b', '\t',
    'q', 'w', 'e', 'r', 't', 'y', 'u', 'i', 'o', 'p', '[', ']', '\n', 0, 'a', 's',
    'd', 'f', 'g', 'h', 'j', 'k', 'l', ';', '\'', '`', 0, '\\', 'z', 'x', 'c', 'v',
    'b', 'n', 'm', ',', '.', '/', 0, '*', 0, ' ', 0, 0, 0, 0, 0, 0, 0, '7',
    '8', '9', '-', '4', '5', '6', '+', '1', '2', '3', '0', '.', 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
};

static const char scancode_to_ascii_shift[128] = {
    0, 27, '!', '@', '#', '$', '%', '^', '&', '*', '(', ')', '_', '+', '\b', '\t',
    'Q', 'W', 'E', 'R', 'T', 'Y', 'U', 'I', 'O', 'P', '{', '}', '\n', 0, 'A', 'S',
    'D', 'F', 'G', 'H', 'J', 'K', 'L', ':', '"', '~', 0, '|', 'Z', 'X', 'C', 'V',
    'B', 'N', 'M', '<', '>', '?', 0, '*', 0, ' ', 0, 0, 0, 0, 0, 0, 0, '7',
    '8', '9', '-', '4', '5', '6', '+', '1', '2', '3', '0', '.', 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
};

// guarda un carácter al buffer (a partir del llamado desde la interrupción)
void keyboard_buffer_push(char c) {
    int next = (kb_head + 1) % KEYBOARD_BUFFER_SIZE;
    if (next != kb_tail) { // buffer no lleno
        keyboard_buffer[kb_head] = c;
        kb_head = next;
       // printChar('*'); // DEBUG: imprime un asterisco cada vez que se guarda algo
    }
}

// Saca un carácter del buffer (llamado desde sys_read)
char nextFromBuffer() {
    if (kb_head == kb_tail)
        return 0; // buffer vacío
    char c = keyboard_buffer[kb_tail];
    kb_tail = (kb_tail + 1) % KEYBOARD_BUFFER_SIZE;
    return c;
}

// Llamado desde el handler cuando se presiona una tecla
void readAndProcess() {
    uint8_t data;
    read(&data);
    if (data == 0x2A || data == 0x36) {
        shift = 1;
        return;
    }
    if (data == 0xAA || data == 0xB6) {
        shift = 0;
        return;
    }
    if(data == 0x01){
        saveRegisters();
    }else if (data < 0x80) {
        char ascii = scancode_to_ascii[data];
		if(shift) {
            ascii = scancode_to_ascii_shift[data];
        }
        if ((ascii >= 32 && ascii <= 126) || ascii == '\n' || ascii == '\b') {
            keyboard_buffer_push(ascii);
            if(ascii == '\b')
              {
              	deleteLastChar();
              }else if(ascii != '\n'){
                printChar(ascii);
            }
        }
    }
}

