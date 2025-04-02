all: clean myshell looper

myshell: myshell.o parser.o
	gcc -g -m32 -Wall -o myshell myshell.o parser.o

myshell.o: myshell.c
	gcc -g -m32 -Wall -c -o myshell.o myshell.c 

parser.o: LineParser.c
	gcc -g -m32 -Wall -c -o parser.o LineParser.c 

looper: looper.o
	gcc -g -m32 -Wall -o looper looper.o

looper.o: looper.c
	gcc -g -m32 -Wall -c looper.c

.PHONY: clean

# Clean rule
clean:
	rm -f *.o myshell looper