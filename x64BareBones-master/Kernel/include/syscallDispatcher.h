#ifndef SYSCALLDISPATCHER_H
#define SYSCALLDISPATCHER_H
#include <stdint.h>

uint64_t sys_read(uint64_t fd, char *buff, uint64_t count);
uint64_t sys_write(uint64_t fd,  const char *buffer, uint64_t count); 
uint64_t syscallDispatcher(uint64_t id, ...);
void drawCircle(uint64_t centerX, uint64_t centerY, uint64_t radius, uint32_t color);
void drawRectangle(uint64_t topLeftX, uint64_t topLeftY, uint64_t width, uint64_t height, uint32_t color);
void clearScreen(uint32_t color);
void setCursor(uint64_t x, uint64_t y);
void deleteLastChar();
void hideCursor();
void showCursor();





#endif