 CC = gcc
 CFLAGS = -g -m32 -Wall

all: task1

task1: task1.o
	$(CC) $(CFLAGS) -o task1 task1.o

task1.o: task1.asm
	nasm -g -f elf -w+all -o task1.o task1.asm

	
.PHONY: clean

clean:
	 rm -f *.o task1
