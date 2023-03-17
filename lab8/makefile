all: task1 task4

task1: ./task1.c
	gcc -Wall -g -m32 ./task1.c -o task1

task4: ./task4.c
	gcc -Wall -g -m32 -fno-pie ./task4.c -o task4

.PHONY: clean
	
clean:
	rm -f ./task1 ./task4
