all: task1 task2 task3

task1: ./task1.c
	gcc -Wall -g -m32 ./task1.c -o task1

task2: ./task2.c
	gcc -Wall -g -m32 ./task2.c -o task2	

task3: ./task3.c
	gcc -Wall -g -m32 ./task3.c -o task3

.PHONY: clean
	
clean:
	rm -f ./task1 ./task2 ./task3
