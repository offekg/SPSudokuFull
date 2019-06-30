#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

#include "game.h"
#include "parser.h"
#include "solver.h"

#define MALLOC_ERROR "Error: malloc has failed"

/*
 * fixed == 1 means cell is fixed; fixed == 0 means cell is not fixed
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

/*
 * Receives dimensions of the wanted board and the blocks in the board.
 * Returns a pointer to a Board struct, with the current game board and solution board set to default (all zeros).
 */
Board* create_blank_board(int boardSize,int blockRows, int blockCols){
	int i, j;
	/* int** solution;
	int total_size = boardSize*boardSize; */
	Board* board;
	Cell** solution;
	Cell** current;

	if((board = (Board*) malloc(sizeof(Board))) == NULL){
		fprintf(stderr,"%s",MALLOC_ERROR);
		return NULL;
	}
	board->board_size = boardSize;
	board->block_rows = blockRows;
	board->block_cols = blockCols;
	board->num_empty_cells_current = boardSize*boardSize;
	board->num_empty_cells_solution = boardSize*boardSize;


	if((solution = (Cell**) malloc(boardSize*sizeof(Cell*))) == NULL){
			fprintf(stderr,"%s",MALLOC_ERROR);
			free(board);
			return NULL;
		}
	if((current = (Cell**) malloc(boardSize*sizeof(Cell*))) == NULL){
		fprintf(stderr,"%s",MALLOC_ERROR);
		free(board);
		return NULL;
	}
	for(i = 0; i < boardSize; i++){
		solution[i] = (Cell*) malloc(boardSize*sizeof(Cell));
		current[i] = (Cell*) malloc(boardSize*sizeof(Cell));
		for(j = 0; j < boardSize; j++){
			createCell(&solution[i][j],0);
			createCell(&current[i][j],0);
		}
	}
	board->solution = solution;
	board->current_board = current;

	return board;
}

void destroy_game_board(Cell** board, int size){
	int i;
	for(i = 0; i < size; i++){
		free(board[i]);
	}
	free(board);
}

void destroyBoard(Board* b){
	destroy_game_board(b->current_board, b->board_size);
	destroy_game_board(b->solution, b->board_size);
}


void printCell(Cell* c){
	if(c->isFixed == 0 && c->value != 0)
		printf(" %d ",c->value);
	else
		if(c->value != 0)
			printf(".%d ",c->value);
		else
			printf("   ");
}

/*

void print_row(Cell** board){

}
*/

/*
 * Prints the given board.
 * if type == 1: prints the boards known solution.
 * otherwise, prints boards current state board.
 */
void printBoard(Board* b, int type){
	int i, j;
	Cell** board;
	char* sep_row;
	int total_row_length = (b->board_size * 3) + ((b->board_size / b->block_cols) * 2) + 1;

	if(type == 1)
		board = b->solution;
	else
		board = b->current_board;

	sep_row = (char*) malloc((total_row_length + 2)*sizeof(char));
	for( i = 0; i < total_row_length; i++ ){
		sep_row[i] ='-';
	}
	sep_row[total_row_length] = '\n';
	sep_row[total_row_length + 1] = '\0';


	for( i = 0; i < b->board_size; i++){
		if( i % b->block_rows == 0 )
			printf("%s",sep_row);
		for( j = 0; j < b->board_size; j++){
			if( j % b->block_cols == 0 )
				printf("| ");
			printCell(&(board[i][j]));
		}
		printf("|\n");
	}
	printf("%s",sep_row);

	free(sep_row);
}

/*
 * Generates a agme board with fixed cells accourding to user input.
 *
 * Gets an already created blank Board.
 * generates a randomized full legal board and stores it in solution.
 */
void generate_user_board(Board* board){
	int fixedCells;
	int row, col;

	printf("Please enter the number of cells to fill [0-80]:\n");
	fixedCells = get_fixed_cells();

	backtracking_solution(board, 1);

	while( fixedCells > 0 ){
		row = rand() % (board->board_size);
		col = rand() % (board->board_size);
		if(board->current_board[row][col].isFixed == 0){
			board->current_board[row][col].value = board->solution[row][col].value;
			board->current_board[row][col].isFixed = 1;
			fixedCells--;
		}
	}
}

void execute_command(Command* command, Board* userBoard) {
	/*
	 * should be completed
	 */
	printf("%d", userBoard->board_size); /* only for the make to pass, should be removed later on. */
	free(command);
}

/*
 * Creates and returns a duplicate of a given game_board. (the actual matrix of cells, not Board).
 */
Cell** copy_game_board(Cell** game_board, int board_size){
	int i, j;
	Cell** copy;
	if((copy = (Cell**) malloc(board_size*sizeof(Cell*))) == NULL){
		fprintf(stderr,"%s",MALLOC_ERROR);
		return NULL;
	}
	for(i = 0; i < board_size; i++){
		copy[i] = (Cell*) malloc(board_size*sizeof(Cell));
		for(j = 0; j < board_size; j++){
			copy[i][j].value = game_board[i][j].value;
			copy[i][j].isFixed = game_board[i][j].isFixed;
		}
	}
	return copy;
}

void validate(Board* b){
	Cell** current_copy = copy_game_board(b->current_board, b->board_size);
	int empty_cells_copy = b->num_empty_cells_current;

	int solvable = backtracking_solution(b, 0);
	if( solvable == 0){
		printf("Validation failed: board is unsolvable\n");
		b->current_board = current_copy;
		b->num_empty_cells_current = empty_cells_copy;
	}
	else{
		printf("Validation passed: board is solvable\n");
		destroy_game_board(b->solution, b->board_size);
		b->solution = b->current_board;
		b->current_board = current_copy;
	}
}
