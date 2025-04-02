
all: clean myELF 

myELF: myELF.o 
	gcc -g -m32 -Wall -o myELF myELF.o 

myELF.o: myELF.c
	gcc -g -m32 -Wall -c -o myELF.o myELF.c 

# Clean rule
clean:
	rm -f *.o myElf 