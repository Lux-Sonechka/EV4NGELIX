#include <stdint.h>
#define VIDEO_ADDRESS 0xb8000
#define MAX_ROWS 25
#define MAX_COLS 80
extern int VGA_COLOR;
#define REG_SCREEN_CTRL 0x3D4
#define REG_SCREEN_DATA 0x3D5

int print_char (char character, int col, int row, char attribute_byte);
void print_at (const char* message, int col, int row);
void kprint (const char* message);
void print_uint (unsigned int n);
void clear_screen (void);
void print_backspace (void);
void delay(int milliseconds);
void set_vga_color(int color);
void msdos_intro (void);
void kprint_typing(const char* message, int delay_ms);
int get_cursor (void);
int get_offset_row (int offset);
int get_offset_col (int offset);
void set_cursor (int offset);
int get_screen_offset (int col, int row);
void kprint_hex(uint32_t num);

#define MAKE_COLOR(fg, bg) ((bg << 4) | (fg))

#define VGA_BLACK         0x00
#define VGA_BLUE          0x01
#define VGA_GREEN         0x02
#define VGA_CYAN          0x03
#define VGA_RED           0x04
#define VGA_MAGENTA       0x05
#define VGA_BROWN         0x06
#define VGA_LIGHT_GRAY    0x07
#define VGA_DARK_GRAY     0x08
#define VGA_LIGHT_BLUE    0x09
#define VGA_LIGHT_GREEN   0x0A
#define VGA_LIGHT_CYAN    0x0B
#define VGA_LIGHT_RED     0x0C
#define VGA_LIGHT_MAGENTA 0x0D
#define VGA_YELLOW        0x0E
#define VGA_WHITE         0x0F
/*
#define COLOR_FILE          VGA_WHITE
#define COLOR_ERROR         VGA_RED
#define COLOR_WARNING       VGA_YELLOW
#define COLOR_INFO          VGA_LIGHT_CYAN
#define COLOR_DEBUG         VGA_LIGHT_GRAY
#define COLOR_PROMPT        VGA_LIGHT_GRAY
#define COLOR_INPUT         VGA_WHITE
#define COLOR_DEFAULT       VGA_GREEN
#define COLOR_SUCCESS       VGA_LIGHT_GREEN*/

extern int COLOR_FILE;
extern int COLOR_ERROR;
extern int COLOR_WARNING;
extern int COLOR_INFO;
extern int COLOR_DEBUG;
extern int COLOR_PROMPT;
extern int COLOR_INPUT;
extern int COLOR_DEFAULT;
extern int COLOR_SUCCESS;