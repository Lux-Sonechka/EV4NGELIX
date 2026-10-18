#include <stdint.h>
#include "screen.h"
#include "../kernel/low_level.h"
#include "../lib/libc/mem.h"
int VGA_COLOR = 0x2; // Cor padrão: verde no fundo preto
int margin_left = 4;  // Margem esquerda (colunas)
int margin_top = 4;   // Margem superior (linhas)

int COLOR_FILE = VGA_LIGHT_GRAY;
int COLOR_ERROR = VGA_RED;
int COLOR_WARNING = VGA_YELLOW;
int COLOR_INFO = VGA_LIGHT_CYAN;
int COLOR_DEBUG = VGA_LIGHT_GRAY;
int COLOR_PROMPT = VGA_LIGHT_RED;
int COLOR_INPUT = VGA_WHITE;
int COLOR_DEFAULT = VGA_GREEN;
int COLOR_SUCCESS = VGA_LIGHT_GREEN;


int get_screen_offset (int col, int row) 
{ 
	return 2 * (row * MAX_COLS + col);
}

void set_cursor (int offset)
{
	offset /= 2;
	port_byte_out(REG_SCREEN_CTRL, 14);
	port_byte_out(REG_SCREEN_DATA, (unsigned char)(offset >> 8));
	port_byte_out(REG_SCREEN_CTRL, 15);
	port_byte_out(REG_SCREEN_DATA, (unsigned char)(offset & 0xff));
}
/*
void msdos_intro_old() {

    // Define as cores
    set_vga_color(0x1F); // Texto branco no fundo azul
	
    // Título
	for(int i = 0; i < MAX_COLS; i++) {
		print_at("#", 28, -2); // Centralizado na linha superior
	}
	
	
    print_at("Welcome to EV4-DOS", 28, 0); // Centralizado na linha superior

    // Texto informativo
    set_rows_color(0, 18, 0x1F); // Texto branco no fundo azul
    print_at("For a short introduction for new users type: about (not funcional yet)", 5, 2);
    print_at("For supported shell commands type: help", 5, 3);

    // Instruções adicionais
    //print_at("To adjust the emulated CPU speed, use ctrl-F11 and ctrl-F12.", 5, 5);
    //print_at("To activate the keymapper ctrl-F1.", 5, 6);
    print_at("For more information read the README file in the EV4 root directory.", 5, 7);

    // Rodapé
    
    print_at("HAVE FUN! <3", 30, 9);
    print_at("@ The EV4 Team - http://www.fake-site.os", 15, 10);

	kprint("\n");

	for(int i = 0; i < MAX_COLS; i++) {
		print_at("#", i, 14); // Centralizado na linha superior
	}
	kprint("\n");
	
}*/
void msdos_intro() {

    set_vga_color(VGA_RED); 

	kprint(" > EVX-DOS PEO BETA  \n");
	kprint("                     \n");
	kprint("     .#######.       \n");
	kprint("  .#* ......  *#.    \n");
	kprint(" #*  ._________ *#   \n");
	kprint(" # . |*          #   \n");
	kprint(" # . |   .***.   #   \n");
	kprint(" # . |  ** 0 **  #   \n");
	kprint(" #.. |   '***'  .#   \n");
	kprint(" #*. |         .*#   \n");
	kprint("  '#*.       .*#'    \n");
	kprint("    '#########'      \n");
	kprint("                     \n");
	kprint(" -- Version 1.0b     \n");
	kprint(" -- Author: Luxxie   \n");
	kprint(" -- Version Date: 2025-05-25 \n");
	kprint(" -- For supported shell commands type: help\n");
	kprint(" -- THANKS FOR USING!!\n");
	kprint(" -- PRAISE THE LIGHT! <3\n");

}
void set_row_color(int row, int color) {
    if (row < 0 || row >= MAX_ROWS) {
        return; // Verifica se a fileira está dentro dos limites
    }

    unsigned char* screen = (unsigned char*) VIDEO_ADDRESS;
    for (int col = 0; col < MAX_COLS; col++) {
        int offset = (row * MAX_COLS + col) * 2; // Calcula o offset na memória de vídeo
        screen[offset + 1] = color; // Define o atributo de cor
    }
}
void set_rows_color(int start_row, int end_row, int color) {
	if (start_row < 0 || end_row >= MAX_ROWS || start_row > end_row) {
		return; // Verifica se as fileiras estão dentro dos limites
	}

	unsigned char* screen = (unsigned char*) VIDEO_ADDRESS;
	for (int row = start_row; row <= end_row; row++) {
		for (int col = 0; col < MAX_COLS; col++) {
			int offset = (row * MAX_COLS + col) * 2; // Calcula o offset na memória de vídeo
			screen[offset + 1] = color; // Define o atributo de cor
		}
	}
}
void set_screen_color(int color) {
    unsigned char* screen = (unsigned char*) VIDEO_ADDRESS;
    for (int i = 0; i < MAX_ROWS * MAX_COLS; i++) {
        screen[i * 2 + 1] = color; // Define o atributo de cor para cada célula
    }
}
void set_cell_color(int row, int col, int color) {
    if (row < 0 || row >= MAX_ROWS || col < 0 || col >= MAX_COLS) {
        return; // Verifica se a célula está dentro dos limites
    }

    unsigned char* screen = (unsigned char*) VIDEO_ADDRESS;
    int offset = (row * MAX_COLS + col) * 2; // Calcula o offset na memória de vídeo
    screen[offset + 1] = color; // Define o atributo de cor
}
int get_cursor ()
{
	port_byte_out(REG_SCREEN_CTRL, 14);
	int offset = port_byte_in(REG_SCREEN_DATA) << 8;
	port_byte_out(REG_SCREEN_CTRL, 15);
	offset += port_byte_in(REG_SCREEN_DATA);
	return offset * 2;
}
void kprint_hex(uint32_t num) {
    char buffer[9]; // Buffer para 8 dígitos hex + null terminator
    for (int i = 7; i >= 0; i--) {
        uint8_t nibble = (num >> (i * 4)) & 0x0F;
        if (nibble < 10) {
            buffer[7 - i] = nibble + '0';
        } else {
            buffer[7 - i] = nibble - 10 + 'A';
        }
    }
    buffer[8] = '\0';
    kprint(buffer);
}
int handle_scrolling (int offset)
{
	if (offset < (2 * MAX_COLS * MAX_ROWS)) {
		return offset;
	}

	int i;
	for (i=1; i < MAX_ROWS; i++) {
		memory_copy((u8 *)(get_screen_offset(0, i) + VIDEO_ADDRESS),
				(u8 *)(get_screen_offset(0, i-1) + VIDEO_ADDRESS),
				MAX_COLS * 2);
	}
	
	char* last_line = (char *)(get_screen_offset(0, MAX_ROWS - 1) + VIDEO_ADDRESS);
	for (i=0; i < MAX_COLS * 2; i++) {
		last_line[i] = 0;
	}

	offset -= 2 * MAX_COLS;

	return offset;
}

int print_char (char character, int col, int row, char attribute_byte)
{
	unsigned char *vidmem = (unsigned char*) VIDEO_ADDRESS;
	if (!attribute_byte) 
		attribute_byte = VGA_COLOR;

	int offset;
	if (col >= 0 && row >= 0)
		offset = get_screen_offset(col, row);
	else
		offset = get_cursor();

	if (character == '\n') {
		int rows = offset / (2 * MAX_COLS);
		offset = get_screen_offset(MAX_COLS, rows);
	} else {
		vidmem[offset] = character;
		vidmem[offset+1] = attribute_byte;
		offset += 2;
	}

	offset = handle_scrolling(offset);
	set_cursor(offset);
	return offset;
}

int get_offset_row (int offset)
{
	return offset / (2 * MAX_COLS);
}

int get_offset_col (int offset)
{
	return (offset - (get_offset_row(offset) * 2 * MAX_COLS)) / 2;
}

void print_at(const char* message, int col, int row)
{
    int offset;

    // Adiciona a margem configurável
    if (col >= 0 && row >= 0) {
        col += margin_left; // Adiciona a margem esquerda
        row += margin_top;  // Adiciona a margem superior
        offset = get_screen_offset(col, row);
    } else {
        offset = get_cursor();
        row = get_offset_row(offset);
        col = get_offset_col(offset);
    }

    int i = 0;
    while (message[i] != 0) {
        offset = print_char(message[i++], col, row, VGA_COLOR);
        row = get_offset_row(offset);
        col = get_offset_col(offset);
    }
}

void kprint(const char* message)
{
	print_at(message, -1, -1);
}

void clear_screen ()
{
	int screen_size = MAX_COLS * MAX_ROWS;
	int i;
	unsigned char* screen = (unsigned char*) VIDEO_ADDRESS;

	for (i=0; i < screen_size; i++) {
		screen[i*2] = ' ';
		screen[i*2 + 1] = VGA_COLOR;
	}
	set_cursor(get_screen_offset(0, 0));
}
void delay(int milliseconds) {
    int ticks = milliseconds * 1000; // Ajuste para a velocidade da CPU
    for (int i = 0; i < ticks; i++) {
        asm volatile("nop"); // Instrução "no operation" para evitar otimizações
    }
}

void print_uint (unsigned int n)
{
	unsigned char zero = 48;
	int reversed = 0;
	int digit_count = 0;
	if (!n) { 
		print_char(zero, -1, -1, VGA_COLOR);
		return;
	}
	while (n > 0) {
		reversed += n % 10;
		reversed *= 10;
		n /= 10;
		digit_count++;
	}
	reversed /= 10;
	while (digit_count-- > 0) {
		print_char(reversed % 10 + zero, -1, -1, VGA_COLOR);
		reversed /= 10;
	}
}

void test_screen (void) 
{
	int row;
	int col;
	char ch = 0;
	char forground = 8;
	char background = 0;
	char attribute;

	for (col = 0; col < MAX_COLS; col++) {
		for (row = 0; row < MAX_ROWS; row++) {
			attribute = (background++ << 8) + forground++;		
			print_char(ch++, col, row, attribute);
		}
	}
}

void print_backspace (void)
{
	int offset = get_cursor() - 2;
	int row = get_offset_row(offset);
	int col = get_offset_col(offset);
	print_char(' ', col, row, VGA_COLOR);
	set_cursor(offset);
}
