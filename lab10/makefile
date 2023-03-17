all: my_loader

my_loader: start.o loader.o startup.o
	ld -o my_loader loader.o startup.o start.o -L/usr/lib32 -lc -T linking_script -dynamic-linker /lib32/ld-linux.so.2
	
loader.o: loader.c
	gcc -g -m32 -Wall -c loader.c -o loader.o

start.o: start.s
	nasm -g -f elf -w+all -o start.o start.s  

startup: startup.o
	gcc -g -m32 -Wall -o startup startup.o

startup.o: startup.s
	nasm -g -f elf -w+all -o startup.o startup.s  

.PHONY: clean

clean:
	rm -f *.o my_loader
