all: encoder

encoder: encoder.o 
	gcc -g -m32 -Wall -o encoder encoder.o

encoder.o: encoder.c
	gcc -g -m32 -Wall -c -o encoder.o encoder.c 

# Clean rule
clean:
	rm -f *.o encoder