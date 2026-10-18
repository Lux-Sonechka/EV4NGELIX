#define VGA_WIDTH 320
#define VGA_HEIGHT 200

#include "video.h"

int mouse_x = 160, mouse_y = 100;
unsigned char* fb = (unsigned char*)0xA0000;

void draw_pixel(int x, int y, unsigned int color) {
    fb[y * 320 + x] = color;
}


void draw_bitmap(int x, int y, int width, int height, const unsigned char* bitmap) {
    for (int j = 0; j < height; j++) {
        for (int i = 0; i < width; i++) {
            int pixel_x = x + i;
            int pixel_y = y + j;

            // Ensure the pixel is within screen bounds
            if (pixel_x >= 0 && pixel_x < VGA_WIDTH && pixel_y >= 0 && pixel_y < VGA_HEIGHT) {
                unsigned int color = bitmap[j * width + i];
                draw_pixel(pixel_x, pixel_y, color);
            }
        }
    }
}