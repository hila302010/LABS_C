all: bin/cw

bin/cw: bin/count-words.o 
	gcc -g -m32 -Wall -o bin/cw bin/count-words.o

bin/count-words.o: src/count-words.c
	gcc -g -m32 -Wall -c -o bin/count-words.o src/count-words.c 

# Clean rule
clean:
	rm -f *.o bin/count-words