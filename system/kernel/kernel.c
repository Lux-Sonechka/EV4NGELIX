#include <stdint.h>
#include "../drivers/screen.h"
#include "../cpu/isr.h"
#include "../lib/libc/string.h"
#include "../lib/libc/mem.h"
#include "../kernel/low_level.h"
#include "../kernel/fat16.h"
#include "../lib/libc/ascii.h"
#include "../kernel/shell/shell.h"
#include "../kernel/ram/ram.h"
#include  "../drivers/misc/net.h"
#include "../drivers/video.h"
#include "../kernel/shell/exe.h"

int full_intro = 1;

char *user = "Luxxie";
char *dir = "root";



void kernel_main (void) {
    unsigned char* fb = (unsigned char*)0xA0000;
    
    draw_pixel(100, 100, VGA_RED);
    unsigned char heart_bitmap[64] = {
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x04, 0x04, 0x00, 0x00, 0x04, 0x04, 0x00,
        0x04, 0x04, 0x04, 0x04, 0x04, VGA_WHITE, 0x04, 0x04,
        0x04, 0x04, 0x04, 0x04, 0x04, 0x04, VGA_WHITE, 0x04,
        0x00, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x00,
        0x00, 0x00, 0x04, 0x04, 0x04, 0x04, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x04, 0x04, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
    };
    draw_bitmap(50, 50, 8, 8, heart_bitmap);
    while (1);
}

void kernel_main_old (void) 
{
	
    clear_screen();
    heap_init();
    

	if (full_intro == 1) {
		set_vga_color(VGA_MAGENTA);
        ascii_print("P", 0, 0);
        set_vga_color(VGA_LIGHT_GREEN);
        ascii_print("E", 0, 5);
        set_vga_color(VGA_LIGHT_CYAN);
        ascii_print("O", 0, 10);

        set_vga_color(VGA_LIGHT_GRAY);
        ascii_print("DOS", 1, 17);
        set_vga_color(VGA_GREEN);

        kprint("\n");

		kprint_typing("# EVX V1-beta booting... \n", 10000);
		delay(300000);
		kprint_typing("# Done!\n", 10000);
		kprint_typing("# Loading Boot Sector Reader...\n", 10000);
		//list_files();
		delay(300000);

		kprint_typing("# Loading shell system...\n", 10000);
		

		delay(300000);
		clear_screen();
	}
	
	msdos_intro();
    set_vga_color(VGA_LIGHT_CYAN);
    print_at("@ EVX V1-beta - Luxxie", 25, 0);
    set_vga_color(VGA_LIGHT_RED);
    print_at("@ Welcome to EVX V1-beta", 25, 2);
	print_at("@ For supported shell commands type: help", 25, 3);
	print_at("@ For more information read the README file.", 25, 4);
	print_at("@ HAVE FUN! <3", 25, 6);
	print_at("@ The EVX Team - http://www.fake-site.os\n", 25, 7);
	
    kprint("\n");
	set_vga_color(VGA_WHITE);
	
	isr_install();
	irq_install();

	COLOR_DEFAULT = VGA_RED;
	set_vga_color(COLOR_DEFAULT);
    int offset_x = get_screen_offset(1, 20);
    set_cursor(offset_x);
	prompt();
    net_init();
    

    /*video_clear_screen();
    video_set_mode(0x13); // Modo gráfico 320x200
    draw_rectangle(0, 0, 320, 200, VGA_BLUE); // Limpa a tela com azul
    draw_circle(160, 100, 50, VGA_RED); // Desenha um círculo vermelho no centro da tela
    video_draw_text(10, 10, "A!", 0x15); // Desenha texto na tela*/
    //draw_window(NULL); // Desenha uma janela (ainda não implementada)
    // Aqui você pode adicionar mais chamadas de funções de vídeo
	//test_info();
	//test_ata_read_sector();
}

// ... chame test_ata_read_sector() em algum lugar do seu código para testar
void set_vga_color(int color) {
    VGA_COLOR = color;
}
void kprint_typing(const char* message, int delay_ms) {
    int i = 0;
    while (message[i] != '\0') {
        print_char(message[i], -1, -1, VGA_COLOR); // Imprime o caractere
        delay(delay_ms); // Aguarda o tempo especificado
        i++;
    }
}
