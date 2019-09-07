#ifndef SOLVER_H_
#define SOLVER_H_

#include "game.h"


/*
 * Checks if it is legal to enter value in the board[row][col].
 * if is_random == 1: checks solution board.
 * else: checks current board;
 * returns 1 if legal, 0 if not
 */
int check_valid_value(Board* b, int value, int row, int col, int is_random);


/*
 * Recursive function that fills given Boards game board with a solution.
 * If is_random == 1: Fills b->solution with a random full board by random backtracking.
 * If is_random == 0: Does validation on current state of b->current_board with deterministic backtracking.
 * 					  If no possible solution exists with current_board's state, function returns 0.
 * On success - returns 1 and updates b->solution with the new solution found.
 */
int backtracking_solution(Board* b, int is_random);




#endif /* SOLVER_H_ */
