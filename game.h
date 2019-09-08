#ifndef GAME_H_
#define GAME_H_

#include "parser.h"


typedef enum game_mode {
	INIT_MODE, EDIT_MODE, SOLVE_MODE
}game_mode;

extern game_mode current_mode;
extern int mark_errors;


/*
 * Struct: Cell
 * 		Used to represent a cell in the board.
 *
 * 		value: an integer with the cell's current valuel
 * 		isFixed: represents if the cell's value is fixed or not. 1 means it's fixed, 0 means it's not.
 * 		isError: represents if the cell's current value creates an error regarding another cell in the board.
 */
typedef struct cell_t{
	int value;
	int isFixed;
	int isError;

} Cell;


/*
 * Struct: Board
 * 		Represents a sudoku board.
 *
 * 		current_board: a 2D Cell array, representing the state of the actual board the user tries to solve.
 * 		solution: a 2D Cell array, representing a possible solution to the game board.
 * 		board_size: the dimension of the game board - amount of rows and of columns.
 * 		block_rows: the amount of rows in one block.
 * 		block_cols: the amount of columns in one block.
 * 		num_empty_cells_current:  the amount of empty cells the current_board has at given time.
 * 		num_empty_cells_solution: for use while computing initial backtrack solution.
 * 								  the amount of empty cells left in solution board.
 */
typedef struct board_t{
	Cell** current_board;
	Cell** solution;
	int board_size; /* number of rows and columns of game board */
	int block_rows; /* m; number of rows in one block */
	int block_cols; /* n; number of columns in one block */
	int num_empty_cells_current;
	int num_empty_cells_solution;
} Board;


/*
 * Receives dimensions of the wanted board and the blocks in the board.
 * Returns a pointer to a Board struct, with the current game board and solution board set to default (all zeros).
 */
Board* create_blank_board(int blockRows, int blockCols);


Cell** copy_game_board(Cell** game_board, int board_size);

/*
 * Generates a game board with fixed cells according to user input.
 *
 * Gets a pointer to an already created blank Board.
 * generates a randomized full legal board and stores it in solution.
 */
void generate_user_board(Board *originalBoard);

/*
 *Recieves given command from user, and implements it appropriately.
 */
void execute_command(Command* command);

/*
 * Gets a pointer to a cell, and initializes it with given value.
 * fixed == 1 means cell is fixed; fixed == 0 means cell is not fixed
 * Initializes as non fixed cell.
 */
void createCell(Cell* cell,int val);

/*
 * Destroys properly a given game board, freeing all allocated resources.
 */
void destroy_game_board(Cell** board, int size);

/*
 * Destroys properly a given Board, freeing all allocated resources.
 */
void destroyBoard(Board* b);

/*
 * Prints the given board.
 * if type == 1: prints the board's known solution.
 * otherwise, prints board's current state board.
 */
void printBoard(Board* b, int type);

/*
 * Creates and returns a duplicate of a given game_board. (the actual matrix of cells, not Board).
 */
Cell** copy_game_board(Cell** game_board, int board_size);


/*
 * Prints to the prompt the opening greeting and initial instructions to the user.
 */
void opening_message();


/*
 * Exits gracfully from game
 */
void exit_game(Board* board);


#endif
