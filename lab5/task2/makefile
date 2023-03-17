CC = gcc
CFLAGS = -m32 -Wall -ansi -c -nostdlib -fno-stack-protector

all: main

main: main.o util.o start.o
	ld -m elf_i386 start.o main.o util.o -o main

main.o: main.c
	$(CC) $(CFLAGS) main.c -o main.o

util.o: util.c
	$(CC) $(CFLAGS) util.c -o util.o	

start.o: start.asm
	nasm -f elf32 start.asm -o start.o 
  
.PHONY: clean

clean:
	 rm -f *.o main
