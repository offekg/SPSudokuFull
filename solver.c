# include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

# include "game.h"


/*
 * Checks if it is legal to enter value in the board[row][col].
 * if is_random == 1: checks solution board.
 * else: checks current board;
 * returns 1 if legal, 0 if not
 */
int check_valid_value(Board* b, int value, int row, int col, int is_random){
	int i, j;
	int block_start_row, block_start_col;
	Cell** game_board;

	if( is_random == 1 )
		game_board = b->solution;
	else
		game_board = b->current_board;

	//check for exiting cell with same value in same row or collumn
	for( i = 0; i < b->board_size; i++ ){
		if(game_board[row][i].value == value)
			return 0;
		if(game_board[i][col].value == value)
			return 0;
	}

	//check for exiting cell with same value in same block
	block_start_row = (row/b->block_rows) * b->block_rows;
	block_start_col = (col/b->block_cols) * b->block_cols;
	for( i = block_start_row; i < (block_start_row + b->block_rows); i++){
		for( j = block_start_col; j < (block_start_col + b->block_cols); j++){
			if(game_board[i][j].value == value)
				return 0;
		}
	}

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
		if(check_valid_value(b,value,row,col,is_random) == 1){
			//printf("  value %d is legal.\n",value);
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
	for(i = index_chosen; i < options[0] - 1; i++){
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
	//======================================
	if( *relevent_empty_cells == 0 )  //Stopping condition - if relevent board has no more empty cells, return success
		return 1;

	for( i = 0; i < b->board_size; i++ ){         //find first empty cell
		for( j = 0; j < b->board_size; j++ ){
			if( (game_board[i][j].value == 0) && (game_board[i][j].isFixed == 0) ){ //do we need to check fixed?
				//printf("Checking options for %d,%d\n",i,j);
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
					//printf("  there are %d options left in %d,%d.\n", cur_num_options,i,j);
					game_board[i][j].value = options[index_chosen];
					*relevent_empty_cells -= 1;
					if( backtracking_solution(b,is_random) == 1 ){
						free(options);
						return 1;
					}
					//else means failed to find solution with [i][j] = options[index_chosen].
					*relevent_empty_cells += 1;
					if(is_random == 1)
						remove_option(options, index_chosen);
				}
					//No legal solution for current state of board. Will backtrack.
					game_board[i][j].value = 0;
					//printf("Backtracking from %d,%d\n",i,j);
					free(options);
					return 0;
			}
		}
	}

	return 1;
}

Board* generateRandomBoard() {
	return NULL;
}

