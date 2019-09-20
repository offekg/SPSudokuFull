/*
 * The "solver" module contains all functions that have to calculate actual sudoku game logic,
 * other than functions that use ilp.
 */

#ifndef SOLVER_H_
#define SOLVER_H_

#include "game.h"



/*
 * Checks if it is legal to enter value in the board[row][col].
 * if only_fixed == 1: checks only compared to fixed cells.
 * else: checks current board;
 * returns 1 if legal, 0 if not
 */
int check_valid_value(Board* b, int value, int row, int col, int only_fixed);

/*
 * For use in random backtrack.
 * Get's a certain cell in game board, and returns list of possible options for that cell.
 * At options[0] is the amount of options found
 * If is_random == 1: chooses only valid options. else: gives all options 1-9.
 * returned value needs to be freed after use!!!
 */
int* generate_options(Board* b, int row, int col, int is_random);

/*
 * Function checks and marks if the current value of a given cell (by row, col)
 * is erroneous with regards to other cells. Also marks other cells that clash with it.
 * Returns 1 if no errors found. 0 if cells were marked.
 * Fixed cells can not be erroneous (so they are not marked).
 */
int mark_erroneous_cells(Board* board, int row, int col);

/*
 * Function checks given board for erroneous cells.
 * Returns 1 if there are errors, 0 if there are none.
 */
int check_board_errors(Board* b);

/*
 * Functions recieves a board, and returns the number of possible solutions for
 * the board's current state, using Exhaustive Backtracking
 */
int num_solutions(Board* b);

/*
 * Function tries (up to 1000 times) to generate a random solvable board into the given board.
 * x: amount of random cells to randomly fill before running ilp.
 * y: amount of cells to leave filled in the final board.
 * Returns 1 if successful, 0 otherwise.
 * For use of the GENERATE command.
 */
int generate(Board* board,int x, int y);


#endif /* SOLVER_H_ */
