#ifndef VIDEODRIVER_H
#define VIDEODRIVER_H
#include <stdint.h>
void putPixel(uint32_t hexColor, uint64_t x, uint64_t y);
uint32_t getAdaptiveTextColor(uint32_t backgroundColor);
void printChar(char c);
void printString(const char *str);
void deleteLastChar();
void clearScreen(uint32_t backgroundColor);
void setCursor(uint64_t x, uint64_t y);
void newLine();
void drawCursor();
void deleteCursor();
#endif