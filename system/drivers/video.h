#include <stdint.h>
#include <stddef.h>



void draw_pixel(int x, int y, unsigned int color);
void update_mouse_position(int dx, int dy);
void draw_bitmap(int x, int y, int width, int height, const unsigned char* bitmap);