 CC = gcc
 CFLAGS = -g -m32 -Wall

all: task2


task2: task2.o
	$(CC) $(CFLAGS) -o task2 task2.o

task2.o: task2.asm
	nasm -g -f elf -w+all -o task2.o task2.asm

.PHONY: clean

clean:
	 rm -f *.o task2
