CC = gcc
OBJS = main.o main_aux.o board_utils.o game.o parser.o solver.o gurobi_utils.o linked_list.o stack.o
EXEC = sudoku-console
COMP_FLAG = -ansi -Wall -Wextra -Werror -pedantic-errors
GUROBI_COMP = -I/usr/local/lib/gurobi563/include
GUROBI_LIB = -L/usr/local/lib/gurobi563/lib -lgurobi56

all: $(EXEC)

$(EXEC): $(OBJS)
	$(CC) $(OBJS) $(GUROBI_LIB) -o $@ -lm
main.o: main.c main_aux.h game.h solver.h parser.h SPBufferset.h board_utils.h
	$(CC) $(COMP_FLAG) -c $*.c
main_aux.o: main_aux.c main_aux.h game.h board_utils.h
	$(CC) $(COMP_FLAG) -c $*.c
board_utils.o: board_utils.c board_utils.h game.c parser.h solver.h linked_list.h
	$(CC) $(COMP_FLAG) -c $*.c
game.o: game.c game.h parser.h solver.h stack.h linked_list.h gurobi_utils.h
	$(CC) $(COMP_FLAG) -c $*.c
parser.o: parser.c parser.h game.h main_aux.h
	$(CC) $(COMP_FLAG) -c $*.c
solver.o: solver.c solver.h game.h board_utils.h stack.h
	$(CC) $(COMP_FLAG) -c $*.c
gurobi_utils.o: gurobi_utils.c gurobi_utils.h game.h solver.h
	$(CC) $(COMP_FLAGS) $(GUROBI_COMP) -c $*.cS
linked_list.o: linked_list.c linked_list.h board_utils.h stack.h
	$(CC) $(COMP_FLAG) -c $*.c
stack.o: stack.c stack.h board_utils.h
	$(CC) $(COMP_FLAG) -c $*.c

clean:
	rm -f $(OBJS) $(EXEC)
