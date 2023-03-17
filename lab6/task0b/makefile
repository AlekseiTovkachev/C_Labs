 CC = gcc
 CFLAGS = -g -m32 -Wall

all: looper

looper: LineParser.o looper.o
	$(CC) $(CFLAGS) -o looper LineParser.o looper.o

LineParser.o: LineParser.c
	$(CC) $(CFLAGS) -c LineParser.c -o LineParser.o

looper.o: looper.c
	$(CC) $(CFLAGS) -c looper.c -o looper.o			


.PHONY: clean

clean:
	 rm -f *.o myshell
