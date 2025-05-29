#include <keyboardDriver.h>
#include <stdint.h>
#include <videoDriver.h>

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
    if(scancode_to_ascii[data] !=0){
        printChar(scancode_to_ascii[data], 0xFFFFFF);
    }
}