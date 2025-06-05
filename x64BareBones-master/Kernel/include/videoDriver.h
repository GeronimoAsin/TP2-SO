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
void drawRectangle(uint64_t topLeftX, uint64_t topLeftY, uint64_t width, uint64_t height, uint32_t color);
void drawCircle(uint64_t centerX, uint64_t centerY, uint64_t radius, uint32_t color);
void drawChar(char c);
void redrawScreenFromBuffer();
void clearScreenPixels(uint32_t backgroundColor); 
void increaseFontSize();
void decreaseFontSize();
void drawCursor();
void clearCursor();
void hideCursor();
void showCursor();
#endif