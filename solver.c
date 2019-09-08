#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

#include "game.h"


/*
 * Checks if it is legal to enter value in the board[row][col].
 * if is_random == 1: checks solution board.
 * if only_fixed == 1: checks only compared to fixed cells.
 * else: checks current board;
 * returns 1 if legal, 0 if not
 */
int check_valid_value(Board* b, int value, int row, int col, int is_random, int only_fixed){
	int i, j;
	int block_start_row, block_start_col;
	Cell** game_board;

	if(value > b->board_size || value < 0){
		//printf("Problem 0\n");
		return 0;
	}
	if(value == 0)
		return 1;

	if( is_random == 1 )
		game_board = b->solution;
	else
		game_board = b->current_board;

	/*
	 * check for exiting cell with same value in same row or column
	 */
	for( i = 0; i < b->board_size; i++ ){
		if(game_board[row][i].value == value && i != col){
			//printf("Problem 1\n");
			if(only_fixed == 0 || game_board[row][i].isFixed == 1)
				return 0;
		}
		if(game_board[i][col].value == value && i != row){
			//printf("Problem 2\n");
			if(only_fixed == 0 || game_board[i][col].isFixed == 1)
				return 0;
		}
	}

	/*
	 * check for exiting cell with same value in same block
	 */
	block_start_row = (row/b->block_rows) * b->block_rows;
	block_start_col = (col/b->block_cols) * b->block_cols;
	for( i = block_start_row; i < (block_start_row + b->block_rows); i++){
		for( j = block_start_col; j < (block_start_col + b->block_cols); j++){
			if(game_board[i][j].value == value && (i != row || j != col)){
				//printf("Problem 3\n");
				if(only_fixed == 0 || game_board[i][j].isFixed == 1)
					return 0;
			}
		}
	}

	return 1;
}

/*
 * Function checks and marks if the current value of a given cell (by row, col)
 * is erroneous with regards to other cells. Also marks other cells that clash with it.
 * Returns 1 if no errors found. 0 if cells were marked.
 * Fixed cells can not be erroneous (so they are not marked).
 */
int mark_erroneous_cells(Board* b,int row, int col){
	Cell** game_board = b->current_board;
	Cell* checked_cell = &(game_board[row][col]);
	int value = checked_cell->value;
	int i, j;
	int block_start_row, block_start_col;

	if(value == 0)
		return 1;
	if(game_board[row][col].isFixed == 1)
		return 1;
	/*
	 * check for clash in same row or column
	 */
	for( i = 0; i < b->board_size; i++ ){
		if(game_board[row][i].value == value && i != col){
			//printf("Problem 1\n");
			checked_cell->isError = 1;
			if(game_board[row][i].isFixed == 0)
				game_board[row][i].isError = 1;
		}
		if(game_board[i][col].value == value && i != row){
			//printf("Problem 2\n");
			checked_cell->isError = 1;
			if(game_board[i][col].isFixed == 0)
				game_board[i][col].isError = 1;
		}
	}

	/*
	 * check for exiting cell with same value in same block
	 */
	block_start_row = (row/b->block_rows) * b->block_rows;
	block_start_col = (col/b->block_cols) * b->block_cols;
	for( i = block_start_row; i < (block_start_row + b->block_rows); i++){
		for( j = block_start_col; j < (block_start_col + b->block_cols); j++){
			if(game_board[i][j].value == value && (i != row || j != col)){
				//printf("Problem 3\n");
				checked_cell->isError = 1;
				if(game_board[i][j].isFixed == 0)
					game_board[i][j].isError = 1;
			}
		}
	}
	if(checked_cell->isError == 1)
		return 0;
	return 1;
}
/*
 * For use in random backtrack.
 * Get's a certain cell in game board, and returns list of possible options for that cell.
 * At options[0] is the amount of options found
 * If is_random == 1: chooses only valid options. else: gives all options 1-9.
 * returned value needs to be freed after use!!!
 */
int* generate_options(Board* b, int row, int col, int is_random){
	int* options;
	int value;
	int count = 0;

	options = (int*) malloc(10 * sizeof(int));
	for(value = 1; value <= b->board_size; value++){
		if(check_valid_value(b,value,row,col,is_random,0) == 1){
			/* printf("  value %d is legal.\n",value); */
			count++;
			options[count] = value;
		}
	}
	options[0] = count;
	if(count < 9)
		options = realloc(options,(count+1)*sizeof(int));
	return options;
}

/*
 * Removes from the options array the option at index_chosen, and updates acordingly.
 */
void remove_option(int* options, int index_chosen){
	int i;
	for(i = index_chosen; i < options[0]; i++){
		options[i] = options[i+1];
	}
	options[0]--;
}

/*
 * Recursive function that fills given Boards game board with a solution.
 * If is_random == 1: Fills b->solution with a random full board by random backtracking.
 * If is_random == 0: Does validation on current state of b->current_board with deterministic backtracking.
 * 					  If no possible solution exists with current_board's state, function returns 0.
 * On success - returns 1 and updates b->solution with the new solution found.
 */
int backtracking_solution(Board* b, int is_random){
	int i,j,k;
	int index_chosen;
	int* options;
	int num_options;
	int cur_num_options;
	Cell** game_board;
	int* relevent_empty_cells;

	if( is_random == 1 ){
		game_board = b->solution;
		relevent_empty_cells = &(b->num_empty_cells_solution);
	}
	else{
		game_board = b->current_board;
		relevent_empty_cells = &(b->num_empty_cells_current);
	}
	/* ====================================== */
	 /* Stopping condition - if relevent board has no more empty cells, return success */
	if( *relevent_empty_cells == 0 )
		return 1;

	/* find first empty cell, find options to fill it and try them */
	for( i = 0; i < b->board_size; i++ ){
		for( j = 0; j < b->board_size; j++ ){
			if( (game_board[i][j].value == 0) && (game_board[i][j].isFixed == 0) ){ /* do we need to check fixed? */
				/* printf("Checking options for %d,%d\n",i,j); */
				options = generate_options(b, i, j, is_random);

				num_options = options[0];
				for( k = 1; k <= num_options; k++ ){
					if(is_random == 1)
						if( (cur_num_options = options[0]) != 1)
							index_chosen = (rand() % cur_num_options) + 1;
						else
							index_chosen = 1;
					else
						index_chosen = k;

					/*printf("num options for %d,%d: %d\n",i,j,cur_num_options);
					printf("options: ");
					for(l = 1; l <= cur_num_options; l++){
						printf("%d ",options[l]);
					}

					printf("\n");*/

					game_board[i][j].value = options[index_chosen];
					*relevent_empty_cells -= 1;
					if( backtracking_solution(b,is_random) == 1 ){
						free(options);
						return 1;
					}
					/* if code reaches here it means it failed to find solution with [i][j] = options[index_chosen]. */
					*relevent_empty_cells += 1;
					if(is_random == 1){
						/*printf("***removing %d***\n",options[index_chosen]);*/
						remove_option(options, index_chosen);
					}


				}
				/* No legal solution for current state of board. Will backtrack. */
				game_board[i][j].value = 0;
				free(options);
				return 0;
			}
		}
	}

	return 1;
}

