CC = gcc
CFLAGS = -Wall
SRC = test.c vector.c vectorSort.c
OBJ = $(SRC:.c=.o)
EXEC = vector

all: depend $(EXEC) clean

depend:
	gcc $(SRC) -MM > .depend

$(EXEC): $(OBJ)
	$(CC) $(CFLAGS) $(OBJ) -o test

runtest:
	./test

clean:
	rm *.o
	rm .depend

-include .depend