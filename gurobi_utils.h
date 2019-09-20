/*
 * gurobi_utils.h
 *
 *  Created on: Sep 15, 2019
 *      Author: offek
 */

#ifndef GUROBI_UTILS_H_
#define GUROBI_UTILS_H_

/*
 * Function uses ilp to try and find a solution to the given board.
 * If a solutions is found, returns 1. if everything ran through and no solution found, returns -1.
 * If an error accured, returns 0;
 * When given save_solution as 1, the found solution (if exists) is saved on the given board.
 */
int find_ILP_solution(Board* board, int save_solution);

#endif /* GUROBI_UTILS_H_ */
