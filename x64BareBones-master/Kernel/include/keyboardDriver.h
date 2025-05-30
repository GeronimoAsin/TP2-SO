#ifndef KEYBOARDDRIVER_H
#define KEYBOARDDRIVER_H

void readAndProcess();
char nextFromBuffer();
void keyboard_buffer_push(char c);
#endif