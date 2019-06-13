#ifndef GAME_H_
#define GAME_H_

typedef struct cell_t{
	int value;
	int isFixed;

} Cell;

Cell* createCell(int val, int fixed);
void destroyCell(Cell* cell);

typdef struct board_t{
	Cell** current_board;
	int solution[][];
	int board_size;
	int block_size;
} Board;


Board* createBoard(int board_size, int block_size);
void destroyBoard(Board* b);

void initializeBoard();









#endif
