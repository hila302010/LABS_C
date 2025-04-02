all: bin/main

bin/main: bin/main.o bin/numbers.o bin/add.o
	gcc -g -m32 -Wall -o bin/main bin/main.o bin/numbers.o bin/add.o 
    
bin/main.o: src/main.c
	gcc -g -m32 -Wall -c -o bin/main.o src/main.c 

bin/numbers.o: src/numbers.c
	gcc -g -m32 -Wall -c -o bin/numbers.o src/numbers.c

bin/add.o: src/add.s
	nasm -g -f elf32 -w+all -o bin/add.o src/add.s

# Clean rule
clean:
	rm -f *.o bin/main