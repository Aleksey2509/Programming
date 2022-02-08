CC = gcc
CFLAGS = -Wall
SRC = test.c vector.c vectorSort.c
OBJ = $(SRC:.c=.o)
EXEC = vector

.PHONY: all clean

all: depend $(EXEC) clean

depend:
	gcc $(SRC) -MM > .depend

$(EXEC): $(OBJ)
	$(CC) $(OBJ) -o test

runtest:
	./test

clean:
	rm *.o
	rm .depend

-include .depend