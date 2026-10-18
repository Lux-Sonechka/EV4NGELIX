C_SOURCES = $(wildcard kernel/*.c drivers/*.c cpu/*.c lib/libc/*.c kernel/shell/*.c kernel/user/*.c drivers/misc/*.c kernel/ram/*.c doomgeneric/*.c)
HEADERS = $(wildcard kernel/*.h drivers/*.h cpu/*.h lib/libc/*.h kernel/shell/*.h kernel/user/*.h drivers/misc/*.h kernel/ram/*.h doomgeneric/*.h)

OBJ = ${C_SOURCES:.c=.o cpu/interrupt.o}
OBJ_LIB = ${C_SOURCES:.c=.o}

CC = gcc

# The -fno-pie is used here in order to make sure the compiler does
# not generate position independant executables. On Fedora my compiler
# did not generate position independent executables and things worked
# fine without this flag, but switching to Arch Linux, things broke.
# Now adding this flag to explicitly state that position independent
# executable should be stopped.
# sudo $(EMU) -machine pc -fda $(OS) -drive file=$(DISK),format=raw,if=ide -boot a

CCFLAGS = -m32 -g -Wall -fno-pie -fno-stack-protector
LD = ld
AS = nasm
DRIVE = /dev/sdb1
EMU = qemu-system-i386
DISK = disk.img
MNT_DIR = /mnt
OS = SEELE.IMG
EMUFLAGS = -device isa-debug-exit,iobase=0xf4,iosize=0x04 -fda

all: minios

minios: arch/x86/boot.bin kernel.bin
	sudo cat $^ > $(OS)
	sudo dd if=/dev/zero of=$(DISK) bs=512 count=32780
	sudo mkfs.fat -F 16 $(DISK)
	sudo mount -o loop $(DISK) $(MNT_DIR)
	sudo mkdir -p $(MNT_DIR)
	sudo cp -r ./mnt/* $(MNT_DIR)
	sudo umount $(MNT_DIR)

kernel.bin: kernel/kernel_entry.o ${OBJ}
	$(LD) -m elf_i386 -o $@ -Ttext 0x1000 $^ --oformat binary

%.o: %.c ${HEADERS}
	$(CC) $(CCFLAGS) -ffreestanding -c $< -o $@

%.o: %.asm
	$(AS) $< -f elf32 -o $@
	
%.bin: %.asm
	$(AS) $< -f bin -I '../../16bit/' -o $@
lib.a:
	ar rcs lib.a ./drivers/screen.o ./kernel/low_level.o ./lib/libc/mem.o
clean:
	rm -rf *.bin *.dis *.map *.o minios	
	rm -rf kernel/*.o arch/x86/*.bin drivers/*.o cpu/*.o lib/libc/*.o
	rm -rf $(DISK) $(OS)

kernel.dis: kernel.bin
	ndisasm -b 32 $< > $@
run: all
	sudo $(EMU) -machine pc \
	-drive file=$(OS),format=raw,if=floppy \
	-drive file=$(DISK),format=raw,if=ide \
	-boot a
