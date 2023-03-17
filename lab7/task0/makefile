 CC = gcc
 CFLAGS = -g -m32 -Wall

all: mypipe

#myshell: myshell.o LineParser.o
#	$(CC) $(CFLAGS) -o myshell myshell.o LineParser.o

mypipe: mypipe.c
	$(CC) $(CFLAGS) -o mypipe mypipe.c

#myshell.o: myshell.c
#	$(CC) $(CFLAGS) -c myshell.c -o myshell.o

#LineParser.o: LineParser.c
#	$(CC) $(CFLAGS) -c LineParser.c -o LineParser.o


.PHONY: clean

clean:
	 rm -f *.o myshell
