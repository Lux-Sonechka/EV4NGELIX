#include "keyboard.h"
#include "../kernel/low_level.h"
#include "../kernel/kernel.h"
#include "../cpu/isr.h"
#include "screen.h"
#include "keymap.h"
#include "../kernel/shell/shell.h"

#define SC_MAX 58
#define BACKSPACE 0x0E
#define ENTER 0x1C
#define SHIFT 0x2A
#define SHIFT_ALT 0x36
#define CTRL 0x1D
#define ALT 0x38
#define ESC 0x01
#define TAB 0x0F
#define SPACE 0x39
#define F1 0x3B
#define F2 0x3C
#define F3 0x3D
#define F4 0x3E
#define F5 0x3F
#define F6 0x40
#define F7 0x41
#define F8 0x42
#define F9 0x43
#define F10 0x44
#define F11 0x57
#define F12 0x58
#define INSERT 0x52
#define DELETE 0x53
#define HOME 0x47
#define END 0x4F
#define PAGE_UP 0x49
#define PAGE_DOWN 0x51
#define ARROW_UP 0x48
#define ARROW_DOWN 0x50
#define ARROW_LEFT 0x4B
#define ARROW_RIGHT 0x4D
#define NUM_LOCK 0x45
#define CAPS_LOCK 0x3A

static char key_buffer[256];
static int shift_pressed = 0;
static int caps_lock = 0;

static const char keyboard_key_shift[SC_MAX] = {
    '\0', '\0', '!', '@', '#', '$', '%', '^', '&', '*', '(', ')', '_', '+', '\b', '\t',
    'Q', 'W', 'E', 'R', 'T', 'Y', 'U', 'I', 'O', 'P', '{', '}', '\n', '\0', 'A', 'S',
    'D', 'F', 'G', 'H', 'J', 'K', 'L', ':', '"', '~', '\0', '|', 'Z', 'X', 'C', 'V',
    'B', 'N', 'M', '<', '>', ':', '\0', '\0', '\0', ' '
};

static void keyboard_callback (registers_t regs)
{
	u8 scancode = port_byte_in(0x60);

	if (scancode == 0x2A || scancode == 0x36) { // Shift pressionado
		shift_pressed = 1;
		return;
	} else if (scancode == 0xAA || scancode == 0xB6) { // Shift solto
		shift_pressed = 0;
		return;
	}
	
	if(scancode > SC_MAX)
		return;
	if(scancode == BACKSPACE || scancode == DELETE) {

		if (scancode == BACKSPACE) {
			// Verifica se o buffer não está vazio
			if (key_buffer[0] != '\0') {
				backspace(key_buffer);
				print_backspace();
			}
		}

		
	} else if(scancode == ENTER) {
		kprint("\n");
		user_input(key_buffer);
		key_buffer[0] = '\0';
	}else if (scancode == CAPS_LOCK) { // Caps Lock pressionado
		caps_lock = !caps_lock; // Alterna o estado do Caps Lock
		// kprint(caps_lock ? "Caps Lock On\n" : "Caps Lock Off\n");
		return;
	} 
	else {
		char letter = keyboard_key[(int)scancode];
	
		// Escolhe o mapeamento correto com base no estado do Shift
		if (shift_pressed) {
			letter = keyboard_key_shift[(int)scancode];
		} else {
			letter = keyboard_key[(int)scancode];
		}
	
		// Ajusta para maiúsculas se Caps Lock estiver ativado
		if (caps_lock && letter >= 'a' && letter <= 'z') {
			letter -= 32; // Converte para maiúscula (ASCII)
		} else if (!caps_lock && letter >= 'A' && letter <= 'Z') {
			letter += 32; // Converte para minúscula (ASCII)
		}
	
		char str[2] = {letter, '\0'};
		append(key_buffer, letter);
		kprint(str);
	}
}

void init_keyboard (void)
{
	register_interrupt_handler(IRQ1, keyboard_callback);
}
