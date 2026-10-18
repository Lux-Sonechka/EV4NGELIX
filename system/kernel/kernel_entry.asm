
global _start;
[bits 32]


_start:
	;mov al, 0x13 ; 320x200x256 8-bit mode
	[extern kernel_main]
	call kernel_main
	jmp $
