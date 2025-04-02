
# all: task1

# task1: task1.o util.o
# 	ld -m elf_i386 task1.o util.o -o task1

# task1.o: task1.s
# 	nasm -f elf32 task1.s -o task1.o

# util.o: util.c util.h
# 	gcc -m32 -c util.c -o util.o

# # Clean rule
# clean:
# 	r -f*.o task1
all: start

start: start.o util.o main.o
	ld -m elf_i386 start.o util.o main.o -o start

start.o: start.s
	nasm -f elf32 start.s -o start.o

util.o: util.c util.h
	gcc -m32 -c util.c -o util.o

main.o: main.c 
	gcc -m32 -c main.c -o main.o

# Clean rule
clean:
	rm -f *.o 
