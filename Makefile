all: simcpu

simcpu: simcpu.o
	gcc -o simcpu simcpu.o

simcpu.o: simcpu.c simcpu.h
	gcc -Wall -std=c11 -g -c simcpu.c

clean:
	rm -f simcpu
	rm -f *.o

