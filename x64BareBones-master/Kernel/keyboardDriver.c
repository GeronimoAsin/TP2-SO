#include "include/keyboardDriver.h"
#include <stdint.h>
#include "include/videoDriver.h"
#include "include/naiveConsole.h"

#define KEYBOARD_BUFFER_SIZE 128

//buffer circular de teclado
//guardo 2 indices para saber donde guardar y donde leer el proximo caracter
static char keyboard_buffer[KEYBOARD_BUFFER_SIZE];
static int kb_head = 0, kb_tail = 0;

extern void read(uint8_t *data); // función asm


static const char scancode_to_ascii[128] = {
    0, 27, '1', '2', '3', '4', '5', '6', '7', '8', '9', '0', '-', '=', '\b', '\t',
    'q', 'w', 'e', 'r', 't', 'y', 'u', 'i', 'o', 'p', '[', ']', '\n', 0, 'a', 's',
    'd', 'f', 'g', 'h', 'j', 'k', 'l', ';', '\'', '`', 0, '\\', 'z', 'x', 'c', 'v',
    'b', 'n', 'm', ',', '.', '/', 0, '*', 0, ' ', 0, 0, 0, 0, 0, 0, 0, '7',
    '8', '9', '-', '4', '5', '6', '+', '1', '2', '3', '0', '.', 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
};

// guarda un carácter al buffer (a partir del llamado desde la interrupción)
void keyboard_buffer_push(char c) {
    int next = (kb_head + 1) % KEYBOARD_BUFFER_SIZE;
    if (next != kb_tail) { // buffer no lleno
        keyboard_buffer[kb_head] = c;
        kb_head = next;
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
    if (data < 0x80 && scancode_to_ascii[data] != 0) {
        keyboard_buffer_push(scancode_to_ascii[data]);
        printChar(scancode_to_ascii[data]); // eco en pantalla
    }
}


/*
Implementación inicial sin buffer: 
#include "include/keyboardDriver.h"
#include <stdint.h>
#include "include/videoDriver.h"
#include "include/naiveConsole.h"

extern void read(uint8_t *data); // funcion asm


void readAndProcess(){
    uint8_t  data;
    read(&data);
    char scancode_to_ascii[128] = {
        0,   27,  '1',  '2',  '3',  '4',  '5',  '6',   // 0x00 - 0x07
    '7',  '8',  '9',  '0',  '-',  '=',  '\b', '\t', // 0x08 - 0x0F
    'q',  'w',  'e',  'r',  't',  'y',  'u',  'i',   // 0x10 - 0x17
    'o',  'p',  '[',  ']',  '\n',  0,   'a',  's',   // 0x18 - 0x1F (0 = Ctrl)
    'd',  'f',  'g',  'h',  'j',  'k',  'l',  ';',   // 0x20 - 0x27
    '\'',  '`',  0,  '\\',  'z',  'x',  'c',  'v',    // 0x28 - 0x2F (0 = LShift)
    'b',  'n',  'm',  ',',  '.',  '/',  0,   '*',    // 0x30 - 0x37 (0 = RShift)
        0,   ' ',  0,    0,    0,    0,    0,    0,     // 0x38 - 0x3F (0s = Alt, Caps, F1–F4)
        0,    0,    0,    0,    0,    0,    0,    '7',  // 0x40 - 0x47
    '8',  '9',  '-',  '4',  '5',  '6',  '+',  '1',   // 0x48 - 0x4F
    '2',  '3',  '0',  '.',  0,    0,    0,    0,     // 0x50 - 0x57 (F11, F12, etc.)
        0,    0,    0,    0,    0,    0,    0,    0     // 0x58 - 0x5F
    };
    if(data < 0x80 && scancode_to_ascii[data] !=0 ){
        ncPrintChar(scancode_to_ascii[data]);
        printChar(scancode_to_ascii[data]);
    }
}

*/