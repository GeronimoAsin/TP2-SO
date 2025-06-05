#ifndef SYSCALLDISPATCHER_H
#define SYSCALLDISPATCHER_H
#include <stdint.h>

uint64_t sys_read(uint64_t fd, char *buff, uint64_t count);
uint64_t sys_write(uint64_t fd,  const char *buffer, uint64_t count); 
uint64_t syscallDispatcher(uint64_t id, ...);
void drawCircle(int x, int y, int radius, uint32_t color);
void drawRectangle(int x, int y, int width, int height, uint32_t color);
void clearScreen(uint32_t color);
void setCursor(int x, int y);
void deleteLastChar();
void hideCursor();
void showCursor();





#endif