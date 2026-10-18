#include <stdint.h>
#include "../../kernel/shell/shell.h"
#include "../../drivers/screen.h"
#include "../../cpu/isr.h"
#include "../../lib/libc/string.h"
#include "../../lib/libc/mem.h"
#include "../../kernel/low_level.h"
#include "../../kernel/fat16.h"
#include "../../kernel/kernel.h"
#include "../../kernel/shell/exe.h"


void user_input (char *input)
{

	char* cmd = strtok(input, " ");
	char* arg = strtok(NULL, " ");

	if(strcmp(input, "clear") == 0) {
		clear_screen();
	} 
	else if(strcmp(input, "help") == 0 || strcmp(input, "?") == 0 || strcmp(input, "about") == 0) {

		if (arg) {
			if (strcmp(arg, "file") == 0) {
				kprint("File commands:\n");
				kprint(" read <filename> - show a file's content.\n");
			}
			else if (strcmp(arg, "general") == 0) {
				kprint("General commands:\n");
				kprint(" clear - clear the screen\n");
				kprint(" help - show this help message\n");
				kprint(" end - stop the CPU\n");
				kprint(" ex or evx <filename> - load an executable file.\n");
			}
			else if (strcmp(arg, "disk") == 0) {
				kprint("Disk commands:\n");
				kprint(" dir or root - show files in the current directory\n");
				kprint(" read <filename> - show a file's content.\n");
				kprint(" mode <real or protected> - Changes operational mode.\nIf no arguments, shows current mode.\n");
			}
			else if (strcmp(arg, "memory") == 0) {
				kprint("Memory commands:\n");
				kprint(" meminfo - show memory information.\n");
				kprint(" mode <real or protected> - Changes operational mode.\nIf no arguments, shows current mode.\n");
			}
			else {
				set_vga_color(COLOR_ERROR);
				kprint("Error: ");
				set_vga_color(COLOR_DEFAULT);
				kprint("Unknown command category.");
			}
		}
		else {
			
			kprint("Help: ");
			set_vga_color(COLOR_INFO);
			kprint("This is a simple shell for the PEO-DOS system.\n");
			set_vga_color(COLOR_PROMPT);
			kprint("You can use commands like 'help', 'clear', 'dir', 'read', etc.\n");
			kprint("For more information, type 'help <command category>'\n");
			set_vga_color(COLOR_INFO);
			kprint("For example: help file, help disk, help memory, help general.\n");
		}

		/*kprint("Available commands:\n");
		kprint(" clear - clear the screen\n");
		kprint(" help - show this help message\n");
		kprint(" end - stop the CPU\n");
		kprint(" dir or root - show files in the current directory\n");
		kprint(" read <filename> - show a file's content.\n");
		
		*/

	} else if(strcmp(input, "end") == 0) {
		kprint("Stopping the CPU!\n");
		asm volatile("hlt");
	}
	else if(strcmp(input, "dir") == 0 || strcmp(input, "root") == 0) {
		list_files();
	}
	else if(strcmp(input, "billie") == 0) {
		kprint("I love you, Billie! >w<\n");
	}
	else if(strcmp(input, "mode") == 0) {
		if (arg) {

		}
		else {
			 unsigned int cr0;
			__asm__ volatile ("mov %%cr0, %0" : "=r" (cr0));
			if (cr0 & 1) {
				kprint("Procted mode on!\n");
			} else {
				// modo real
				kprint("Real mode on!\n");
			}
		}
	}
	else if(strcmp(input, "read") == 0) {
		if (arg) {
			char name[8], ext[3];
			parse_filename(arg, name, ext);
			//kprint(name);
			read_file(name, ext);

		} else {
			kprint("Testing without argument...");
		}
	}
	else if(strcmp(input, "ex") == 0 || strcmp(input, "evx") == 0) {
		if (arg) {
			char name[8], ext[3];
			parse_filename(arg, name, ext);
			//kprint(name);
			//kprint(".");
			//kprint(ext);
			//kprint("\n");
			int ex = exe_load(name, ext);
			kprint("Exit code: ");
			print_uint(ex);
			kprint("\n");
			
		} else {
			kprint("Testing without argument...");
		}
	}
	else if (strcmp(input, "meminfo") == 0) {
		meminfo();
	}
	else if (strcmp(input, "peo") == 0) {
		msg_about();
	}
	else {
		set_vga_color(COLOR_ERROR);
		kprint("Error: ");
		set_vga_color(COLOR_DEFAULT);
		kprint("Unknown command.");
		kprint("(");
		kprint(input);
		kprint(")\n");
		set_vga_color(COLOR_INFO);
		kprint("Try help for a list of commands.\n");
		set_vga_color(COLOR_DEFAULT);
	}

	prompt();
}
void prompt() {

	// Define a cor do prompt
	set_vga_color(COLOR_PROMPT);

	// Imprime o prompt
	kprint("\n");
	kprint(user);
	kprint("@");
	kprint(dir);
	kprint("$ ");
	
	// Restaura a cor padrão
	set_vga_color(COLOR_DEFAULT);
	
	// Imprime o cursor

	kprint("> ");
}
void parse_filename(const char *input, char *name_out, char *ext_out) {
    // Limpa os buffers com espaços (formato 8.3 exige espaços, não '\0')
    for (int i = 0; i < 8; i++) name_out[i] = ' ';
    for (int i = 0; i < 3; i++) ext_out[i] = ' ';

    const char *dot = strchr(input, '.');
    int name_len = dot ? (dot - input) : strlen(input);

    // Copia o nome, converte pra maiúsculo
    for (int i = 0; i < name_len && i < 8; i++) {
        name_out[i] = toupper(input[i]);
    }

    // Copia a extensão, se existir
    if (dot) {
        for (int i = 0; i < 3 && dot[1 + i]; i++) {
            ext_out[i] = toupper(dot[1 + i]);
        }
    }
}


void msg_about() {

	kprint("- PEO-DOS - The EVX Team\n");
	kprint("	Version BETA TEST 1.0\n");
	kprint("	Author: Luxxie\n");
	kprint("	Email: lux.sonechka@gmail.com\n");
	kprint("	Website: www.peodos.com\n");
	kprint("	License: GNU GPL v3\n");

}
