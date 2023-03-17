 CC = gcc #compiler
 CFLAGS = -g -m32 -Wall

all: task3

task3: task3.o
	$(CC) $(CFLAGS) -o task3 task3.o	

task3.o: task3.asm
	nasm -g -f elf -w+all -o task3.o task3.asm

.PHONY: clean

clean:
	 rm -f *.o task1 task2 task3 task3b
