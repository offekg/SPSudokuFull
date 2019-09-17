#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

#include "game.h"
#include "parser.h"
#include "solver.h"

//#define MALLOC_ERROR "Error: malloc has failed\nNow exiting game"


/*
 * Gets a pointer to a cell, and initializes it with given value.
 * fixed == 1 means cell is fixed; fixed == 0 means cell is not fixed
 * Initializes as non fixed cell.
 */
void createCell(Cell* cell,int val){
	cell->value = val;
	cell->isFixed = 0;
	cell->isError = 0;
	cell->options = NULL;
}

/*
 * Receives dimensions of the wanted board and the blocks in the board.
 * Returns a pointer to a Board struct, with the current game board and solution board set to default (all zeros).
 */
Board* create_blank_board(int blockCols, int blockRows){
	int i, j;
	Board* board;
	Cell** solution;
	Cell** current;

	if((board = (Board*) malloc(sizeof(Board))) == NULL){
		fprintf(stderr,"%s",MALLOC_ERROR);
		exit(0);
	}
	board->block_rows = blockRows;
	board->block_cols = blockCols;
	board->board_size = blockCols*blockRows;
	board->num_empty_cells_current = board->board_size*board->board_size;
	board->num_empty_cells_solution = board->board_size*board->board_size;


	if((solution = (Cell**) malloc(board->board_size*sizeof(Cell*))) == NULL){
			fprintf(stderr,"%s",MALLOC_ERROR);
			free(board);
			exit(0);
		}
	if((current = (Cell**) malloc(board->board_size*sizeof(Cell*))) == NULL){
		fprintf(stderr,"%s",MALLOC_ERROR);
		free(board);
		exit(0);
	}
	for(i = 0; i < board->board_size; i++){
		solution[i] = (Cell*) malloc(board->board_size*sizeof(Cell));
		current[i] = (Cell*) malloc(board->board_size*sizeof(Cell));
		if(current[i] == NULL || solution[i] == NULL){
			fprintf(stderr,"%s",MALLOC_ERROR);
			free(board);
			exit(0);
		}
		for(j = 0; j < board->board_size; j++){
			createCell(&solution[i][j],0);
			createCell(&current[i][j],0);
		}
	}
	board->solution = solution;
	board->current_board = current;

	return board;
}

/*
 * Destroys properly a given game board, freeing all allocated resources.
 */
void destroy_game_board(Cell** board, int size){
	int i, j;
	for(i = 0; i < size; i++){
		for(j = 0; j < size; j++){
			if(board[i][j].options != NULL)
				free(board[i][j].options);
		}
		free(board[i]);
	}
	free(board);
}

/*
 * Destroys properly a given Board, freeing all allocated resources.
 */
void destroyBoard(Board* b){
	destroy_game_board(b->current_board, b->board_size);
	destroy_game_board(b->solution, b->board_size);
	free(b);
}


/*
 * Prints a single cell, acording to the sudoku board format and the status of the cell.
 */
void printCell(Cell* c){

	if( (c->isFixed == 0) && (c->isError == 0 || (current_mode == SOLVE_MODE && mark_errors == 0)) && (c->value != 0) )
		printf(" %2d ",c->value);
	else
		if(c->value != 0 && c->isFixed == 1)
			printf(" %2d.",c->value);
		else
			if(c->value != 0 && c->isError == 1 && (current_mode == EDIT_MODE || mark_errors == 1))
				printf(" %2d*",c->value);
			else
				printf("    ");
}

void printIsError(Board* b){
		int i, j;
		Cell** board;
		char* sep_row;
		int total_row_length = (4* b->board_size) + b->block_rows + 1;
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
					printf("|");
				printf("  %d ",board[i][j].isError);
			}
			printf("|\n");
		}
		printf("%s",sep_row);

		free(sep_row);

}


/*
 * Prints the given board.
 * if type == 1: prints the board's known solution.
 * otherwise, prints board's current state board.
 */
void printBoard(Board* b, int type){
	int i, j;
	Cell** board;
	char* sep_row;
	int total_row_length = (4* b->board_size) + b->block_rows + 1;

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
				printf("|");
			printCell(&(board[i][j]));
		}
		printf("|\n");
	}
	printf("%s",sep_row);

	free(sep_row);
}

/*
 * Generates a game board with fixed cells according to user input.
 *
 * Gets a pointer to an already created blank Board.
 * generates a randomized full legal board and stores it in solution.
 */
void generate_user_board(Board* board){
	int fixedCells;
	int row, col;

	printf("Please enter the number of cells to fill [0-80]:\n");
	fixedCells = get_fixed_cells(board);

	backtracking_solution(board, 1);

	while( fixedCells > 0 ){
		col = rand() % (board->board_size);
		row = rand() % (board->board_size);
		if(board->current_board[row][col].isFixed == 0){
			board->current_board[row][col].value = board->solution[row][col].value;
			board->current_board[row][col].isFixed = 1;
			fixedCells--;
			board->num_empty_cells_current--;
		}
	}
}

/*
 * Creates and returns a duplicate of a given game_board. (the actual matrix of cells, not Board).
 */
Cell** copy_game_board(Cell** game_board, int board_size){
	int i, j;
	Cell** copy;
	if(game_board == NULL)
		return NULL;

	if((copy = (Cell**) malloc(board_size*sizeof(Cell*))) == NULL){
		fprintf(stderr,"%s",MALLOC_ERROR);
		exit(0);
	}
	for(i = 0; i < board_size; i++){
		if( (copy[i] = (Cell*) malloc(board_size*sizeof(Cell))) == NULL){
			fprintf(stderr,"%s",MALLOC_ERROR);
			exit(0);
		}
		for(j = 0; j < board_size; j++){
			copy[i][j].value = game_board[i][j].value;
			copy[i][j].isFixed = game_board[i][j].isFixed;
			copy[i][j].isError = game_board[i][j].isError;
		}
	}
	return copy;
}

/*
 * Creates and returns a duplicate of a given Board.
 */
Board* copy_Board(Board* b){
	Board* copy_board;
	if(b == NULL)
		return NULL;

	if((copy_board = (Board*) malloc(sizeof(Board))) == NULL){
		fprintf(stderr,"%s",MALLOC_ERROR);
		exit(0);
	}

	copy_board->block_rows = b->block_rows;
	copy_board->block_cols = b->block_cols;
	copy_board->board_size = b->board_size;
	copy_board->num_empty_cells_current = b->num_empty_cells_current;
	copy_board->num_empty_cells_solution = b->num_empty_cells_solution;

	copy_board->current_board = copy_game_board(b->current_board,b->board_size);
	copy_board->solution = copy_game_board(b->solution,b->board_size);

	return copy_board;
}

