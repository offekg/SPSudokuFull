#ifndef GAME_H_
#define GAME_H_

typedef struct cell_t{
	int value;
	int isFixed;

} Cell;

void createCell(Cell* cell,int val);
void destroyCell(Cell* cell);


typedef struct board_t{
	Cell** current_board;
	int** solution;
	int board_size; //number of rows and columns of game board
	int block_rows;
	int block_cols;
} Board;


Board* createBoard(int boardSize,int blockRows, int blockCols);
void destroyBoard(Board* b);

void initializeBoard();









#endif
