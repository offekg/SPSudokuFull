#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "game.h"
#include "parser.h"
#include "solver.h"

/*
 * fixed == 1 means cell is fixed; ==0 means cell is not fixed
 */
Cell* createCell(int val, int fixed){
	Cell* cell = (Cell*) malloc(sizeof(Cell));
	cell->value = val;
	if(fixed == 1)
		cell->isFixed = 1;
	else
		cell->isFixed = 0;
	return cell;
}

void destroyCell(Cell* cell){
	if(!cell)
		return;
	free(cell);
}

void initializeBoard(){
	int fixedCells;
	char[][] sudoku;
	printf("Please enter the number of cells to fill [0-80]:\n")
	fixedCells = get_fixed_cells();
	sudoku = generateBoard();
	randomlySelectBoard(sudoku, fixedCells);
}

void randomlySelectBoard(char[][] sudoku, int fixedCells){

}
