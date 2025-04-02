all: menu

menu: menu.o 
	gcc -g -m32 -Wall -o menu menu.o

menu.o: menu_map.c
	gcc -g -m32 -Wall -c -o menu.o menu_map.c 

# Clean rule
clean:
	rm -f *.o menu