#include <stdint.h>
#include "include/videoDriver.h"
#include "include/bitmap.h"

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

void printChar(char c, uint32_t color) {
	if (currentX >= VBE_mode_info->width || currentY >= VBE_mode_info->height) {
		return;
	}

	if(currentX + 8 > VBE_mode_info->width || currentY + 16 > VBE_mode_info->height) {
		return;
	}

	uint8_t *glyph = font_bitmap +16 * (c -32);;

	for (int cy = 0; cy < 16; cy++) {
		uint8_t row = glyph[cy];
		for (int cx = 0; cx < 8; cx++) {
			// Verificamos cada bit de la fila, empezando por el más significativo
			if (row & (0x80 >> cx)) {
				putPixel(color, currentX + cx, currentY + cy);
			} else {
				// Si el bit está en 0, dibujamos el color de fondo
				putPixel(0x000000, currentX + cx, currentY + cy);
			}
		}
	}


	currentX += 8;
}


void printString(const char *str, uint32_t color) {
	while (*str) {
		printChar(*str, color);
		str++;
	}
}

void deleteLastChar(uint32_t backgroundColor) {
	if (currentX <= 0) {
		return;
	}

	currentX -= 8;

	printChar(' ', backgroundColor);

	currentX -= 8;
}



void clearScreen(uint32_t backgroundColor) {
	for (uint64_t y = 0; y < VBE_mode_info->height; y++) {
		for (uint64_t x = 0; x < VBE_mode_info->width; x++) {
			putPixel(backgroundColor, x, y);
		}
	}

	currentX = 0;
	currentY = 0;
}


void setCursor(uint64_t x, uint64_t y) {
	currentX = x;
	currentY = y;
}

void newLine() {
	currentX = 0;
	currentY += 16;

	if (currentY >= VBE_mode_info->height) {
		currentY = 0;
	}
}

void drawRectangle(uint64_t topLeftX, uint64_t topLeftY, uint64_t width, uint64_t height, uint32_t color) {
	for (uint64_t y = topLeftY; y < topLeftY + height && y < VBE_mode_info->height; y++) {
		for (uint64_t x = topLeftX; x < topLeftX + width && x < VBE_mode_info->width; x++) {
			putPixel(color, x, y);
		}
	}
}


void drawCircle(uint64_t centerX, uint64_t centerY, uint64_t radius, uint32_t color) {
	int64_t x = 0;
	int64_t y = radius;
	int64_t d = 3 - 2 * radius;

	while (y >= x) {
		// Dibuja líneas horizontales para rellenar el círculo en cada octante
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