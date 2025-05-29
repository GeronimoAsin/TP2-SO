#ifndef VIDEODRIVER_H
#define VIDEODRIVER_H
#include <stdint.h>
void putPixel(uint32_t hexColor, uint64_t x, uint64_t y);
void printChar(char c, uint32_t color);
void printString(const char *str, uint32_t color);
void deleteLastChar(uint32_t backgroundColor);
void clearScreen(uint32_t backgroundColor);
void setCursor(uint64_t x, uint64_t y);
void newLine();

#endif