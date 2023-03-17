 CC = gcc #compiler
 CFLAGS = -g -m32 -Wall

#all: bubblesort hexaPrint
all: virusDetector

bubblesort: bubblesort.c
	$(CC) $(CFLAGS) -o bubblesort bubblesort.c

hexaPrint: hexaPrint.c
	$(CC) $(CFLAGS) -o hexaPrint hexaPrint.c

virusDetector: virusDetector.c
	$(CC) $(CFLAGS) -o virusDetector virusDetector.c

.PHONY: clean

clean:
	 rm -f *.o bubblesort hexaPrint virusDetector