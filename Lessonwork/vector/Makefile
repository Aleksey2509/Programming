CC = gcc
CFLAGS = -c -o
SRC = test.c vector.c

all: depend runtest clear

depend:
	gcc $(SRC) -MM > .depend

compile_test: vector.o test.o
	$(CC) $^ -o test

runtest: vector.o compile_test
	./test

clear:
	rm *.o
	rm test
	rm .depend

-include .depend