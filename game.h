#ifndef GAME_H_
#define GAME_H_

#include "parser.h"


typedef struct cell_t{
	int value;
	int isFixed;

} Cell;


typedef struct board_t{
	Cell** current_board;
	int** solution;
	int board_size; /* number of rows and columns of game board */
	int block_rows;
	int block_cols;
} Board;


Board* createBoard(int boardSize,int blockRows, int blockCols);
Board* create_user_board(Board *originalBoard);

void execute_command(Command* command, Board* userBoard);
void createCell(Cell* cell,int val);
void destroyCell(Cell* cell);
void destroyBoard(Board* b);






#endif
