#ifndef BOARD_UTILS_H_
#define BOARD_UTILS_H_


#define MALLOC_ERROR "Error: malloc has failed\nNow exiting game"

/*
 * Structure: Cell
 * 		Used to represent a cell in the board.
 *
 * 		value: an integer with the cell's current valuel
 * 		isFixed: represents if the cell's value is fixed or not. 1 means it's fixed, 0 means it's not.
 * 		isError: represents if the cell's current value creates an error regarding another cell in the board.
 * 		options: an int array that can save all valid options for a cell when neede.
 * 				 the 0 index saves how many options there are.
 */
typedef struct cell_t{
	int value;
	int isFixed;
	int isError;
	int* options;

} Cell;


/*
 * Structure: Board
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

/*
 * Creates and returns a duplicate of a given game_board. (the actual matrix of cells, not Board).
 */
Cell** copy_game_board(Cell** game_board, int board_size);

/*
 * Generates a game board with fixed cells according to user input.
 *
 * Gets a pointer to an already created blank Board.
 * generates a randomized full legal board and stores it in solution.
 */
void generate_user_board(Board *originalBoard);


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

void printIsError(Board* b);

/*
 * Creates and returns a duplicate of a given game_board. (the actual matrix of cells, not Board).
 */
Cell** copy_game_board(Cell** game_board, int board_size);

/*
 * Creates and returns a duplicate of a given Board.
 */
Board* copy_Board(Board* b);



#endif /* BOARD_UTILS_H_ */
