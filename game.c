#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

#include "game.h"
#include "parser.h"
#include "solver.h"

#define MALLOC_ERROR "Error: malloc has failed"

/*
 * fixed == 1 means cell is fixed; ==0 means cell is not fixed
 */
void createCell(Cell* cell,int val){
	cell->value = val;
	cell->isFixed = 0;
}

void destroyCell(Cell* cell){
	if(!cell)
		return;
	free(cell);
}


Board* createBoard(int boardSize,int blockRows, int blockCols){
	int total_size = boardSize*boardSize;
	Board* board;
	if((board = (Board*) malloc(sizeof(Board))) == NULL){
		fprintf(stderr,"%s on Board\n",MALLOC_ERROR);
		return NULL;
	}
	board->board_size = boardSize;
	board->block_rows = blockRows;
	board->block_cols = blockCols;
	int** solution;
	if((solution = (int**) calloc(total_size,sizeof(int))) == NULL){
		fprintf(stderr,MALLOC_ERROR);
		return NULL;
	}
	board->solution = solution;

	Cell** current;
	if((current = (Cell**) malloc(boardSize*sizeof(Cell*))) == NULL){
		fprintf(stderr,"%s on cell pointers array\n",MALLOC_ERROR);
		free(board);
		return NULL;
	}
	for(int i = 0; i < boardSize; i++){
		current[i] = (Cell*) malloc(boardSize*sizeof(Cell));
		for(int j = 0; j < boardSize; j++){
			createCell(&current[i][j],0);
		}
	}
	board->current_board = current;

	return board;
}

void destroyBoard(Board* b){
	for(int i = 0; i < b->board_size; i++)
		for(int j = 0; j < b->board_size; j++)
			destroyCell(&(b->current_board)[i][j]);
	free(b->current_board);
	free(b->solution);
}
/*
void initializeBoard(){
	int fixedCells;
	char** sudoku;
	printf("Please enter the number of cells to fill [0-80]:\n")
	fixedCells = get_fixed_cells();
	sudoku = generateBoard();
	randomlySelectBoard(sudoku, fixedCells);
}

void randomlySelectBoard(char[][] sudoku, int fixedCells){

}*/
