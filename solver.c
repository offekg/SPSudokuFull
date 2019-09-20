#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

#include "game.h"
#include "board_utils.h"
#include "stack.h"


/*
 * Checks if it is legal to enter value in the board[row][col].
 * if is_random == 1: checks solution board. Otherwise checks current_board.
 * if only_fixed == 1: checks only compared to fixed cells.
 * else: checks current board;
 * returns 1 if legal, 0 if not
 */
int check_valid_value(Board* b, int value, int row, int col, int is_random, int only_fixed){
	int i, j;
	int block_start_row, block_start_col;
	Cell** game_board;

	if(value > b->board_size || value < 0){
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
			if(only_fixed == 0 || game_board[row][i].isFixed == 1)
				return 0;
		}
		if(game_board[i][col].value == value && i != row){
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
				if(only_fixed == 0 || game_board[i][j].isFixed == 1)
					return 0;
			}
		}
	}

	return 1;
}

/*
 * Checks if it is legal to enter value in the board[row][col].
 * if only_fixed == 1: checks only compared to fixed cells.
 * else: checks current board;
 * returns 1 if legal, 0 if not
 */
/*int check_valid_value_new(Cell** game_board,int block_rows,int block_cols, int value, int row, int col, int only_fixed){*/
int check_valid_value_new(Board* b, int value, int row, int col, int only_fixed){
	Cell** game_board = b->current_board;
	int block_rows = b->block_rows;
	int block_cols = b->block_cols;
	int i, j;
	int block_start_row, block_start_col;
	int board_size = block_cols*block_rows;

	if(value > board_size || value < 0){
		return 0;
	}
	if(value == 0)
		return 1;

	/*
	 * check for exiting cell with same value in same row or column
	 */
	for( i = 0; i < board_size; i++ ){
		if(game_board[row][i].value == value && i != col){
			if(only_fixed == 0 || game_board[row][i].isFixed == 1)
				return 0;
		}
		if(game_board[i][col].value == value && i != row){
			if(only_fixed == 0 || game_board[i][col].isFixed == 1)
				return 0;
		}
	}

	/*
	 * check for exiting cell with same value in same block
	 */
	block_start_row = (row/block_rows) * block_rows;
	block_start_col = (col/block_cols) * block_cols;
	for( i = block_start_row; i < (block_start_row + block_rows); i++){
		for( j = block_start_col; j < (block_start_col + block_cols); j++){
			if(game_board[i][j].value == value && (i != row || j != col)){
				if(only_fixed == 0 || game_board[i][j].isFixed == 1)
					return 0;
			}
		}
	}

	return 1;
}

/*
 * Function checks and marks if the current value of a given cell (by regular C row, col)
 * is erroneous with regards to other cells or not. Also marks\unmarks other cells that
 * clash with it.
 * Returns 1 if no errors found. 0 if cells were marked.
 * Fixed cells can not be erroneous (so they are not marked).
 */
/*int mark_erroneous_cells(Cell** game_board,int block_rows,int block_cols,int row, int col){*/
int mark_erroneous_cells(Board* board, int row, int col){
	Cell** game_board = board->current_board;
	Cell* checked_cell = &(game_board[row][col]);
	int board_size = board->board_size;
	int i, j;
	int temp;
	int block_rows = board->block_rows;
	int block_cols = board->block_cols;
	int block_start_row, block_start_col;

	/*if(value == 0)
		return 1;*/
	if(game_board[row][col].isFixed == 1)
		return 1;
	/*
	 * check for clash in same row or column
	 */
	for( i = 0; i < board_size; i++ ){
		temp = game_board[row][i].value;
		game_board[row][i].value = 0;
		if(game_board[row][i].isFixed == 0){
			if(check_valid_value_new(board, temp, row, i, 0) == 1)
				game_board[row][i].isError = 0;
			else
				game_board[row][i].isError = 1;
		}
		game_board[row][i].value = temp;

		temp = game_board[i][col].value;
		game_board[i][col].value = 0;
		if(i != row && game_board[i][col].isFixed == 0){
			if(check_valid_value_new(board, temp, i, col, 0) == 1)
				game_board[i][col].isError = 0;
			else
				game_board[i][col].isError = 1;
		}
		game_board[i][col].value = temp;
	}

	/*
	 * check for exiting cell with same value in same block
	 */
	block_start_row = (row/block_rows) * block_rows;
	block_start_col = (col/block_cols) * block_cols;
	for( i = block_start_row; i < (block_start_row + block_rows); i++){
		for( j = block_start_col; j < (block_start_col + block_cols); j++){
			if( (i != row || j != col) && (game_board[i][j].isFixed == 0) ){
				temp = game_board[i][j].value;
				game_board[i][j].value = 0;
				if(check_valid_value_new(board, temp, i, j, 0) == 1)
					game_board[i][j].isError = 0;
				else
					game_board[i][j].isError = 1;
				game_board[i][j].value = temp;
			}
		}
	}
	if(checked_cell->isError == 1)
		return 0;
	return 1;
}

/*
 * Function checks and marks if the current value of a given cell (by regular C row, col)
 * is erroneous with regards to other cells. Also marks other cells that clash with it.
 * Returns 1 if no errors found. 0 if cells were marked.
 * Fixed cells can not be erroneous (so they are not marked).
 */
int mark_erroneous_cells_old(Cell** game_board,int block_rows,int block_cols,int row, int col){
	Cell* checked_cell = &(game_board[row][col]);
	int board_size = block_cols*block_rows;
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
	for( i = 0; i < board_size; i++ ){
		if( game_board[row][i].value == value && i != col ){
			checked_cell->isError = 1;
			if(game_board[row][i].isFixed == 0)
				game_board[row][i].isError = 1;
		}
		if( game_board[i][col].value == value && i != row ){
			checked_cell->isError = 1;
			if(game_board[i][col].isFixed == 0)
				game_board[i][col].isError = 1;
		}
	}

	/*
	 * check for exiting cell with same value in same block
	 */
	block_start_row = (row/block_rows) * block_rows;
	block_start_col = (col/block_cols) * block_cols;
	for( i = block_start_row; i < (block_start_row + block_rows); i++){
		for( j = block_start_col; j < (block_start_col + block_cols); j++){
			if(game_board[i][j].value == value && (i != row || j != col)){
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
 * Function checks given board for erroneous cells.
 * Returns 1 if there are errors, 0 if there are none.
 */
int check_board_errors(Board* b){
	int i,j;
	for(i = 0; i < b->board_size; i++)
		for(j = 0; j < b->board_size; j++)
			if(b->current_board[i][j].isError == 1)
				return 1;

	return 0;
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
	/*printf("checking col: %d row: %d\n",col+1,row+1);*/
	options = (int*) malloc(10 * sizeof(int));
	if(options == NULL){
		printf(MALLOC_ERROR);
		exit(0);
	}
	for(value = 1; value <= b->board_size; value++){
		if(check_valid_value(b,value,row,col,is_random,0) == 1){
			/*printf("  value %d is legal.\n",value);*/
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
 * Given a board, the function finds the first cell that is empty,
 * and puts it's row and col values in the given pointers.
 * If no empty cell exists, the functions returns 0. If it does, returns 1.
 */
int find_first_empty_cell(Board* b, int* row, int* col){
	int i,j;
	for( i = 0; i < b->board_size; i++ ){
		for( j = 0; j < b->board_size; j++ ){
			if( b->current_board[i][j].value == 0 ){
				*row = i;
				*col = j;
				return 1;
			}
		}
	}
	return 0;
}

/*
 * Function is given a board and a cell's row and col;
 * It finds and returns the next (bigger) valid value to put in the cell, that is closest
 * to the current value.
 * If no bigger value is valid, returns 0.
 * for use in num_solutions
 */
int find_next_valid_value(Board* b, int row, int col){
	int current_value = b->current_board[row][col].value;
	int next_value;
	for(next_value = current_value + 1; next_value <= b->board_size; next_value++){
		/*printf("checking %d, ",next_value);*/
		if(check_valid_value_new(b,next_value,row,col,0) == 1){
			/*printf("\nfound next valid: %d\n",next_value);*/
			return next_value;}
	}
	/*printf("\nnothing is valid,returning 0\n");*/
	return 0;
}


/*
 * Functions recieves a board, and returns the number of possible solutions for
 * the board's current state, using Exhaustive Backtracking on a Stack.
 * should work on a copy
 */
int num_solutions(Board* b){
	Stack* stack;
	StackElem* elem;
	int num_sol = 0;
	int next_value;
	int row, col;

	/* ====================================== */

	if(check_board_errors(b) == 1){
	/*if the board has errors then there is no solution*/
		return 0;
	}

	if(b->num_empty_cells_current == 0)
	/*if there are no errors and no empty cells, then there is one solution*/
		return 1;

	/* find first empty cell, find smallest option to fill it with */
	find_first_empty_cell(b, &row, &col);
	next_value = find_next_valid_value(b,row,col);
	if(next_value == 0){
		return 0;
	}

	stack = initialize_stack();
	push(stack, row, col, next_value);

	while(!is_empty(stack)){
		elem = top(stack);
		set_value_simple(b,elem->row,elem->col,elem->value);
		/*printBoard(b,0);
		printf("count of stack: %d\n",stack->count);
		print_Stack(stack);*/

		if(b->num_empty_cells_current == 0){
		/*Found a solution. now we try to find more by backtracking and incrementing*/
			num_sol++;
			while( ((next_value = find_next_valid_value(b,elem->row,elem->col)) == 0) && (is_empty(stack) != 1) ){
				set_value_simple(b,elem->row,elem->col,0);
				pop(stack);
				free(elem);
				elem = top(stack);
				if(elem == NULL)
					break;
			}
			if( (is_empty(stack)) && (next_value == 0) ){
			/*finished going through all fill options*/
				continue;
			}
			pop(stack);
			push(stack, elem->row, elem->col, next_value);
			free(elem);
		}
		else{
		/*Board not full yet. find next empty cell, and push it to stack with next valid value*/
			find_first_empty_cell(b, &row, &col);
			next_value = find_next_valid_value(b, row, col);
			if(next_value != 0)
				push(stack, row, col, next_value);
			else{
				while( ((next_value = find_next_valid_value(b,elem->row,elem->col)) == 0) && !is_empty(stack) ){
					set_value_simple(b,elem->row,elem->col,0);
					pop(stack);
					free(elem);
					elem = top(stack);
					if(elem == NULL)
						break;
				}
				if( (is_empty(stack)) && (next_value == 0) ){
				/*finished going through all fill options*/
					continue;
				}
				else{
					if(elem->row == stack->top->row && elem->col == stack->top->col)
						pop(stack);
					push(stack, elem->row, elem->col, next_value);
					free(elem);
				}
			}

		}
	}

	destroy_stack(stack);
	return num_sol;
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

