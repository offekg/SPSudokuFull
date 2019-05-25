#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "parser.h"
#include "solver.h"

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
