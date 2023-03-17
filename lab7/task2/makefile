 CC = gcc
 CFLAGS = -g -m32 -Wall

all: mypipeline

mypipeline: mypipeline.c
	$(CC) $(CFLAGS) -o mypipeline mypipeline.c

.PHONY: clean

clean:
	 rm -f *.o mypipeline
