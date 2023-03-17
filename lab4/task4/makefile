 CC = gcc #compiler
 CFLAGS = -g -m32 -Wall
	
task4: task4.o
	$(CC) $(CFLAGS) -o task4 task4.o

task4.o: task4.asm
	nasm -g -f elf -w+all -o task4.o task4.asm

.PHONY: clean

clean:
	 rm -f *.o task4
