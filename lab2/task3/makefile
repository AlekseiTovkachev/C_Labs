 CC = gcc #compiler
 CFLAGS = -g -m32 -Wall

all: menu_map

menu_map: menu_map.c
	$(CC) $(CFLAGS) -o menu_map menu_map.c
  


.PHONY: clean

clean:
	 rm -f *.o menu_map
