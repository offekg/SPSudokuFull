#ifndef SOLVER_H_
#define SOLVER_H_

#include "game.h"

Board* generateRandomBoard();
int backtracking_solution(Board* b, int is_random);
int check_valid_value(Board* b, int value, int row, int col, int is_random);


#endif /* SOLVER_H_ */
