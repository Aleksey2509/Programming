CC = gcc
CFLAGS = -Wall -g
SRC = test.c vector.c vectorSort.c
OBJ = $(SRC:.c=.o)
EXEC = vector
RM = rm -rf

all: $(EXEC) clean

depend:
	gcc $(SRC) -MM > .depend

$(EXEC): $(OBJ)
	$(CC) $(CFLAGS) $(OBJ) -o test

runtest:
	./test

clean:
	$(RM) *.o

-include .depend