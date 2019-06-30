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

void generate_user_board(Board *originalBoard);
void execute_command(Command* command, Board* board);
void createCell(Cell* cell,int val);
void destroyCell(Cell* cell);
void destroyBoard(Board* b);
void printBoard(Board* b, int type);






#endif
