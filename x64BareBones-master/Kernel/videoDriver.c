#include <stdint.h>
#include "include/videoDriver.h"
#include "include/bitmap.h"
#define PROMPT_LENGTH 8
#define PROMPT_X_MIN (PROMPT_LENGTH * 8) 
#define MAX_ROWS 100
#define MAX_COLS 200
char screenBuffer[MAX_ROWS][MAX_COLS];
int bufferRows = 0, bufferCols[MAX_ROWS] = {0};
uint8_t fontWidth = 8;
uint8_t fontHeight = 16;
const uint8_t FONT_WIDTH_MIN = 4;
const uint8_t FONT_HEIGHT_MIN = 8;
const uint8_t FONT_WIDTH_MAX = 32;
const uint8_t FONT_HEIGHT_MAX = 64;

struct vbe_mode_info_structure {
	uint16_t attributes;		// deprecated, only bit 7 should be of interest to you, and it indicates the mode supports a linear frame buffer.
	uint8_t window_a;			// deprecated
	uint8_t window_b;			// deprecated
	uint16_t granularity;		// deprecated; used while calculating bank numbers
	uint16_t window_size;
	uint16_t segment_a;
	uint16_t segment_b;
	uint32_t win_func_ptr;		// deprecated; used to switch banks from protected mode without returning to real mode
	uint16_t pitch;			// number of bytes per horizontal line
	uint16_t width;			// width in pixels
	uint16_t height;			// height in pixels
	uint8_t w_char;			// unused...
	uint8_t y_char;			// ...
	uint8_t planes;
	uint8_t bpp;			// bits per pixel in this mode
	uint8_t banks;			// deprecated; total number of banks in this mode
	uint8_t memory_model;
	uint8_t bank_size;		// deprecated; size of a bank, almost always 64 KB but may be 16 KB...
	uint8_t image_pages;
	uint8_t reserved0;
 
	uint8_t red_mask;
	uint8_t red_position;
	uint8_t green_mask;
	uint8_t green_position;
	uint8_t blue_mask;
	uint8_t blue_position;
	uint8_t reserved_mask;
	uint8_t reserved_position;
	uint8_t direct_color_attributes;
 
	uint32_t framebuffer;		// physical address of the linear frame buffer; write here to draw to the screen
	uint32_t off_screen_mem_off;
	uint16_t off_screen_mem_size;	// size of memory in the framebuffer but not being displayed on the screen
	uint8_t reserved1[206];
} __attribute__ ((packed));

typedef struct vbe_mode_info_structure * VBEInfoPtr;

VBEInfoPtr VBE_mode_info = (VBEInfoPtr) 0x0000000000005C00;

uint64_t currentX = 0;
uint64_t currentY = 0;

void putPixel(uint32_t hexColor, uint64_t x, uint64_t y) {
    uint8_t * framebuffer = (uint8_t *) VBE_mode_info->framebuffer;
    uint64_t offset = (x * ((VBE_mode_info->bpp)/8)) + (y * VBE_mode_info->pitch);
    framebuffer[offset]     =  (hexColor) & 0xFF;
    framebuffer[offset+1]   =  (hexColor >> 8) & 0xFF; 
    framebuffer[offset+2]   =  (hexColor >> 16) & 0xFF;
}

uint16_t getWidth() {
	return VBE_mode_info->width;
}

uint16_t getHeight() {
	return VBE_mode_info->height;
}

uint32_t currentBackgroundColor = 0x000000;

// Función para obtener color de texto adaptativo (blanco para fondo oscuro, negro para fondo claro)
uint32_t getAdaptiveTextColor(uint32_t backgroundColor) {
    uint8_t r = (backgroundColor >> 16) & 0xFF;
    uint8_t g = (backgroundColor >> 8) & 0xFF;
    uint8_t b = backgroundColor & 0xFF;
    uint32_t luminance = (uint32_t)(0.299*r + 0.587*g + 0.114*b);
    return (luminance > 128) ? 0x000000 : 0xFFFFFF;
}

// Funcion que imprime un caracter
void printChar(char c) {
	if (currentX + fontWidth > VBE_mode_info->width) {
		newLine();
	}
	if (currentY + fontHeight > VBE_mode_info->height) {
		currentY = 0;
	}

	if (bufferCols[bufferRows] < MAX_COLS) {
		screenBuffer[bufferRows][bufferCols[bufferRows]++] = c;
	}

	uint32_t textColor = getAdaptiveTextColor(currentBackgroundColor);
	uint8_t *glyph = font_bitmap + 16 * (c - 32);
	for (int cy = 0; cy < fontHeight; cy++) {
		uint8_t row = glyph[cy * 16 / fontHeight];
		for (int cx = 0; cx < fontWidth; cx++) {
			if (row & (0x80 >> (cx * 8 / fontWidth))) {
				putPixel(textColor, currentX + cx, currentY + cy);
			} else {
				putPixel(currentBackgroundColor, currentX + cx, currentY + cy);
			}
		}
	}
	currentX += fontWidth;
}

// Modifica printString para no recibir color
void printString(const char *str) {
    while (*str) {
        printChar(*str);
        str++;
    }
}

// Modifica deleteLastChar para usar el fondo global
void deleteLastChar() {
    int promptXMin = PROMPT_LENGTH * fontWidth;
    if (currentX <= promptXMin) {
        return;
    }

	if (bufferCols[bufferRows] == 0 && bufferRows == 0) {
		return; 
	}

	if (bufferCols[bufferRows] == 0 && bufferRows > 0) {
		bufferRows--;
	}
	if (bufferCols[bufferRows] > 0) {
		bufferCols[bufferRows]--;
	}

	if (currentX >= fontWidth) {
		currentX -= fontWidth;
	} else if (currentY >= fontHeight) {
		currentY -= fontHeight;
		currentX = bufferCols[bufferRows] * fontWidth;
	} else {
		currentX = 0;
		currentY = 0;
	}

	for (int cy = 0; cy < fontHeight; cy++) {
		for (int cx = 0; cx < fontWidth; cx++) {
			putPixel(currentBackgroundColor, currentX + cx, currentY + cy);
		}
	}
}

// clearScreen actualiza el fondo global
void clearScreen(uint32_t backgroundColor) {
	for (uint64_t y = 0; y < VBE_mode_info->height; y++) {
		for (uint64_t x = 0; x < VBE_mode_info->width; x++) {
			putPixel(backgroundColor, x, y);
		}
	}
	currentX = 0;
	currentY = 0;
	currentBackgroundColor = backgroundColor;
	bufferRows = 0;
	bufferCols[0] = 0;
}

// Funcion que actualiza la posición del cursor
void setCursor(uint64_t x, uint64_t y) {
	currentX = x;
	currentY = y;
}

// Funcion que mueve el cursor a la siguiente línea
void newLine() {
	currentX = 0;
	currentY += fontHeight;
	if (currentY + fontHeight > VBE_mode_info->height) {
		clearScreen(currentBackgroundColor);
	} else {
		bufferRows++;
		bufferCols[bufferRows] = 0;
	}
}

// Funcion que dibuja un rectángulo en pantalla
void drawRectangle(uint64_t topLeftX, uint64_t topLeftY, uint64_t width, uint64_t height, uint32_t color) {
	for (uint64_t y = topLeftY; y < topLeftY + height && y < VBE_mode_info->height; y++) {
		for (uint64_t x = topLeftX; x < topLeftX + width && x < VBE_mode_info->width; x++) {
			putPixel(color, x, y);
		}
	}
}

// Función que dibuja un círculo en pantalla 
void drawCircle(uint64_t centerX, uint64_t centerY, uint64_t radius, uint32_t color) {
	int64_t x = 0;
	int64_t y = radius;
	int64_t d = 3 - 2 * radius;

	while (y >= x) {
		for (int64_t i = centerX - x; i <= centerX + x; i++) {
			putPixel(color, i, centerY + y);
			putPixel(color, i, centerY - y);
		}
		for (int64_t i = centerX - y; i <= centerX + y; i++) {
			putPixel(color, i, centerY + x);
			putPixel(color, i, centerY - x);
		}

		x++;
		if (d > 0) {
			y--;
			d = d + 4 * (x - y) + 10;
		} else {
			d = d + 4 * x + 6;
		}
	}
}

// Funcion que dibuja un carácter en pantalla
void drawChar(char c) {
	if (currentX + fontWidth > VBE_mode_info->width) {
		currentX = 0;
		currentY += fontHeight;
	}
	if (currentY + fontHeight > VBE_mode_info->height) {
		return;
	}
	uint32_t textColor = getAdaptiveTextColor(currentBackgroundColor);
	uint8_t *glyph = font_bitmap + 16 * (c - 32);
	for (int cy = 0; cy < fontHeight; cy++) {
		uint8_t row = glyph[cy * 16 / fontHeight];
		for (int cx = 0; cx < fontWidth; cx++) {
			if (row & (0x80 >> (cx * 8 / fontWidth))) {
				putPixel(textColor, currentX + cx, currentY + cy);
			} else {
				putPixel(currentBackgroundColor, currentX + cx, currentY + cy);
			}
		}
	}
	currentX += fontWidth;
}

// Funcion que redibuja la pantalla desde el buffer
void redrawScreenFromBuffer() {
	currentX = 0;
	currentY = 0;
	for (int row = 0; row <= bufferRows; row++) {
		for (int col = 0; col < bufferCols[row]; col++) {
			if (currentY + fontHeight > VBE_mode_info->height) {
				return;
			}
			drawChar(screenBuffer[row][col]);
		}
		if (row != bufferRows) {
			currentX = 0;
			currentY += fontHeight;
		}
	}
}

// Funcion que limpia la pantalla con un color de fondo determinado
void clearScreenPixels(uint32_t backgroundColor) {
	for (uint64_t y = 0; y < VBE_mode_info->height; y++) {
		for (uint64_t x = 0; x < VBE_mode_info->width; x++) {
			putPixel(backgroundColor, x, y);
		}
	}
	currentX = 0;
	currentY = 0;
	currentBackgroundColor = backgroundColor;
}

// Función para aumentar el tamaño de la fuente
void increaseFontSize() {
	if (fontWidth < FONT_WIDTH_MAX && fontHeight < FONT_HEIGHT_MAX) {
		fontWidth *= 2;
		fontHeight *= 2;
		clearScreenPixels(currentBackgroundColor);
		redrawScreenFromBuffer();
	}
}

// Función para disminuir el tamaño de la fuente
void decreaseFontSize() {
	if (fontWidth > FONT_WIDTH_MIN && fontHeight > FONT_HEIGHT_MIN) {
		fontWidth /= 2;
		fontHeight /= 2;
		clearScreenPixels(currentBackgroundColor);
		redrawScreenFromBuffer();
	}
}

static int cursorVisible = 1;

// Función para dibujar el cursor en la posición actual
void drawCursor() {
    if (!cursorVisible) return;
    uint32_t cursorColor = 0xFFFFFF ^ currentBackgroundColor; 
    for (int cy = 0; cy < fontHeight; cy++) {
        for (int cx = 0; cx < fontWidth; cx++) {
            putPixel(cursorColor, currentX + cx, currentY + cy);
        }
    }
}

// Función para limpiar el cursor 
void clearCursor() {
    for (int cy = 0; cy < fontHeight; cy++) {
        for (int cx = 0; cx < fontWidth; cx++) {
            putPixel(currentBackgroundColor, currentX + cx, currentY + cy);
        }
    }
}

// Funciones para ocultar el cursor
void hideCursor() {
    cursorVisible = 0;
    clearCursor();
}

// Función para mostrar el cursor
void showCursor() {
    cursorVisible = 1;
    drawCursor();
}

