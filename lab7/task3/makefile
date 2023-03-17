 CC = gcc
 CFLAGS = -g -m32 -Wall

all: myshell looper

myshell: myshell.o LineParser.o
	$(CC) $(CFLAGS) -o myshell myshell.o LineParser.o

looper: looper.c
	$(CC) $(CFLAGS) -o looper looper.c

myshell.o: myshell.c
	$(CC) $(CFLAGS) -c myshell.c -o myshell.o

LineParser.o: LineParser.c
	$(CC) $(CFLAGS) -c LineParser.c -o LineParser.o


.PHONY: clean

clean:
	 rm -f *.o myshell
