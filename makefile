CC = gcc
OBJS = main.o main_aux.o game.o parser.o solver.o
EXEC = sudoku
COMP_FLAG = -ansi -Wall -Wextra -Werror -pedantic-errors

$(EXEC): $(OBJS)
	$(CC) $(OBJS) -o $@
main.o: main.c main_aux.h game.h solver.h parser.h SPBufferset.h
	$(CC) $(COMP_FLAG) -c $*.c
main_aux.o: main_aux.c main_aux.h game.h 
	$(CC) $(COMP_FLAG) -c $*.c
game.o: game.c game.h parser.h solver.h
	$(CC) $(COMP_FLAG) -c $*.c
parser.o: parser.c parser.h game.h
	$(CC) $(COMP_FLAG) -c $*.c
solver.o: solver.c solver.h game.h
	$(CC) $(COMP_FLAG) -c $*.c
clean:
	rm -f $(OBJS) $(EXEC)
