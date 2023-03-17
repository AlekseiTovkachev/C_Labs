 CC = gcc #compiler
 TARGET = main #targer file name
 CFLAGS = -g -m32 -Wall

 #{-tab-}actions 

 # All Targets 

all: encoder

encoder: encoder.c
	$(CC) $(CFLAGS) -o encoder encoder.c

 #tell make that "clean" is not a file name!
  
.PHONY: clean

#Clean the build directory

clean:
	 rm -f *.o encoder
