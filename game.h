#ifndef GAME_H_
#define GAME_H_

#include "parser.h"


typedef struct cell_t{
	int value;
	int isFixed;

} Cell;


typedef struct board_t{
	Cell** current_board;
	Cell** solution;
	int board_size; /* number of rows and columns of game board */
	int block_rows;
	int block_cols;
	int num_empty_cells_current;
	int num_empty_cells_solution;
} Board;


Board* create_blank_board(int boardSize,int blockRows, int blockCols);
Cell** copy_game_board(Cell** game_board, int board_size);

void generate_user_board(Board *originalBoard);
void execute_command(Command* command, Board* userBoard);
void createCell(Cell* cell,int val);
void destroyCell(Cell* cell);
void destroy_game_board(Cell** board, int size);
void destroyBoard(Board* b);
void printBoard(Board* b, int type);
void validate(Board* b);






#endif
