#!/bin/bash
set -e  # para parar se algum comando falhar

# Passo 1: compilar sem libc e sem startup
gcc -m32 -ffreestanding -c lib.c -o lib.o
ar rcs lib.a lib.o
gcc -m32 -ffreestanding -c main.c -o ./bin/main.o

# Passo 2: linkar com script linker.ld para binário puro
ld -m elf_i386 -T linker.ld -o ./bin/main.bin ./bin/main.o lib.a 

# Passo 3: extrair o binário cru
objcopy -O binary ./bin/main.bin ./bin/main.raw

# Passo 4: gerar o EVX com header e código (usando python script externo)
python3 convert.py ./bin/main.raw program.evx

cp program.evx ../system/mnt/

echo "Programm succesfully compiled and linked! ✨"
