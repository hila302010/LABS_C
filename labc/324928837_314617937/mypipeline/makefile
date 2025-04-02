all: clean mypipeline


mypipeline: mypipeline.o 
	gcc -g -m32 -Wall -o mypipeline mypipeline.o 

mypipeline.o: myPipeLine.c
	gcc -g -m32 -Wall -c -o mypipeline.o myPipeLine.c 

.PHONY: clean

# Clean rule
clean:
	rm -f *.o mypipeline 