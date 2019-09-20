/*
 * The "gurobi_utils" module is in charge of all functions that directly use Gurobi
 * for finding ilp solutions.
 */

#include <stdlib.h>
#include <stdio.h>
#include "gurobi_c.h"
#include "board_utils.h"
#include "game.h"
#include "solver.h"



/*
 * for ilp use.
 * Allocates a 3 dimensional int array for the need variables, with 0 values in all cells.
 * returns the array.
 */
int*** allocate_var_indexs(int size){
	int i, j;
	int*** array = (int***) malloc(size * size * sizeof(int**));

	for(i = 0; i < size; i++){
		array[i] = (int**) malloc(size * sizeof(int*));
		for(j = 0; j < size; j++){
			array[i][j] = (int*) calloc(size, sizeof(int));
		}
	}
	return array;
}

/*
 *Destroys safely the var_index 3d array.
 */
void destroy_var_indexs(int*** array, int size){
	int i, j;

	for(i = 0; i < size; i++){
		for(j = 0; j < size; j++)
			free(array[i][j]);
		free(array[i]);
	}
	free(array);
}

/*
 * Function frees all allocated memory used in the enviroment.
 * Get's all the pointers to be freed.
 */
void free_resorces(GRBenv* env, GRBmodel* model, char* vtype, int*** var_indexs,
					double* obj, int* ind, double* val, double* sol, int board_size){
	destroy_var_indexs(var_indexs, board_size);
	free(vtype);
	free(obj);
	free(ind);
	free(val);
	free(sol);
	GRBfreemodel(model);
	GRBfreeenv(env);
}


/*
 * Function that Creates the Gurobi enviroment, with the GRBenv and GRBmodel given.
 * Returns 1 on success, 0 on failure.
 */
int create_env(GRBenv** env, GRBmodel** model){
	  int       error = 0;

	  /* Create environment - log file is integerLinearPrograming.log */
	  error = GRBloadenv(env, "integerLinearPrograming.log");
	  if (error) {
		  printf("ERROR: %d GRBloadenv(): %s\n", error, GRBgeterrormsg(*env));
		  return 0;
	  }

	  error = GRBsetintparam(*env, GRB_INT_PAR_LOGTOCONSOLE, 0);
	  if (error) {
		  printf("ERROR: %d GRBsetintattr(): %s\n", error, GRBgeterrormsg(*env));
		  GRBfreeenv(*env);
		  return 0;
	  }

	  /* Create an empty model named "mip1" */
	  error = GRBnewmodel(*env, model, "integerLinearPrograming", 0, NULL, NULL, NULL, NULL, NULL);
	  if (error) {
		  printf("ERROR: %d GRBnewmodel(): %s\n", error, GRBgeterrormsg(*env));
		  GRBfreeenv(*env);
		  return 0;
	  }
	  return 1;
}

/*
 * add the variables to the model.
 * returns 1 on success, 0 on failure.
 */
int add_variables(GRBenv** env, GRBmodel** model, int var_amount, double** obj, char** vtype){
	int i;
	int error;

	for(i = 0; i < var_amount; i++){
		(*obj)[i] = 0;
		(*vtype)[i] = GRB_BINARY;
	}
	/* add variables to model */
	 error = GRBaddvars(*model, var_amount, 0, NULL, NULL, NULL, *obj, NULL, NULL, *vtype, NULL);
	 if (error) {
		 printf("ERROR %d GRBaddvars(): %s\n", error, GRBgeterrormsg(*env));
		 return 0;
	 }

	 /* Set model Attribute */
	 error = GRBsetintattr(*model, GRB_INT_ATTR_MODELSENSE, GRB_MAXIMIZE);
	 if (error) {
		 printf("ERROR %d GRBsetintattr(): %s\n", error, GRBgeterrormsg(*env));
		 return 0;
	  }

	  /* update the model - to integrate new variables */
	  error = GRBupdatemodel(*model);
	  if (error) {
		  printf("ERROR %d GRBupdatemodel(): %s\n", error, GRBgeterrormsg(*env));
		  return 0;
	  }

	return 1;
}

/*
 * Function receives all needed to add constraints to the model, for ilp.
 * Returns 1 on success, o on failure.
 */
int add_constraints(Board* board, GRBenv** env, GRBmodel** model, int var_amount,
		 int** ind, double** val, int*** var_indexs){
	int i, j, k, a, b;
	int n = board->block_cols;
	int m = board->block_rows;
	int current;
	int board_size = board->board_size;
	int error;
	Cell* curr_cell;

	/*Constraint 1: each cell has exactly 1 value*/
	for(i = 0; i < board_size; i++)
		for(j = 0; j < board_size; j++){
			curr_cell = &(board->current_board[i][j]);
			if(curr_cell->value == 0){
				for(k = 0; k < curr_cell->options[0]; k++){
					/*printf("    entered third for with option %d\n",curr_cell->options[k+1]);*/
					(*ind)[k] = var_indexs[i][j][curr_cell->options[k+1] - 1] - 1;
					/*printf("    ok with var_indexes. put in %d\n",(*ind)[k]);*/
					(*val)[k] = 1;
				}
				error = GRBaddconstr(*model, k, *ind, *val, GRB_EQUAL, 1.0, "c1");
				if (error) {
					printf("ERROR %d 1st GRBaddconstr(): %s\n", error, GRBgeterrormsg(*env));
					return 0;
				}
				/*printf("success in addcinstr for cell <%d,%d>\n", j+1,i+1);*/
			}
		}

	/*Constraint 2: each row has one of each possible value*/
	for(i = 0; i < board_size; i++)
		for(k = 0; k < board_size; k++){
			current = 0;
			for(j = 0; j < board_size; j++){
				if(var_indexs[i][j][k] > 0){
					(*ind)[current] = var_indexs[i][j][k] - 1;
					(*val)[current] = 1;
					current += 1;
				}
			}
			if(current > 0){
				error = GRBaddconstr(*model, current, *ind, *val, GRB_EQUAL, 1.0, "c2");
				if (error) {
					printf("ERROR %d 2nd GRBaddconstr(): %s\n", error, GRBgeterrormsg(*env));
					return 0;
				}

			}
		}

	/*Constraint 3: each column has one of each possible value*/
	for(j = 0; j < board_size; j++)
		for(k = 0; k < board_size; k++){
			current = 0;
			for(i = 0; i < board_size; i++){
				if(var_indexs[i][j][k] > 0){
					(*ind)[current] = var_indexs[i][j][k] - 1;
					(*val)[current] = 1;
					current += 1;
				}
			}
			if(current > 0){
				error = GRBaddconstr(*model, current, *ind, *val, GRB_EQUAL, 1.0, "c3");
				if (error) {
					printf("ERROR %d 3rd GRBaddconstr(): %s\n", error, GRBgeterrormsg(*env));
					return 0;
				}

			}
		}

	/*Constraint 4: each block has one of each possible value*/
	for(a = 0; a < n; a++) /*block row*/
		for(b = 0; b < m; b++){ /*block column*/
			for(k = 0; k < board_size; k++){ /*checked value*/
				current = 0;
				for(i = a * m; i < (a+1) * m; i++) 			/*going through the block a,b*/
					for(j = b * n; j < (b+1) * n ; j++){
						if(var_indexs[i][j][k] > 0){
							(*ind)[current] = var_indexs[i][j][k] - 1;
							(*val)[current] = 1;
							current += 1;
						}
					}
					if(current > 0){
						error = GRBaddconstr(*model, current, *ind, *val, GRB_EQUAL, 1.0, "c4");
						if (error) {
							printf("ERROR %d 4th GRBaddconstr(): %s\n", error, GRBgeterrormsg(*env));
							return 0;
						}
					}
			}
		}


	return 1;
}


/*
 * Function receives Gurobi's output of solution for the given board
 * and saves the solution on to the board itself.
 */
void save_sol_to_board(Board* board, double* sol, int*** var_indexs){
	int i, j, k;
	int index;
	for(i = 0; i < board->board_size; i++)
		for(j = 0; j < board->board_size; j++){
			for(k = 0; k < board->board_size; k++){
				index = var_indexs[i][j][k];
				if(index > 0 && sol[index-1] == 1.0){
					/*printf("setting cell <%d,%d> to %d\n",j+1,i+1,k+1);*/
					set_value_simple(board, i, j, k + 1);
					break;
				}
			}
		}
}

/*
 * Function uses ilp to try and find a solution to the given board.
 * If a solutions is found, returns 1. if everything ran through and no solution found, returns -1.
 * If an error accured, returns 0;
 * When given save_solution as 1, the found solution (if exists) is saved on the given board.
 */
int find_ILP_solution(Board* board, int save_solution){
	GRBenv   *env   = NULL;
	GRBmodel *model = NULL;
	int error;
	double*   sol;
	int*      ind;
	double*   val;
	double*   obj;
	char*     vtype;
	int       optimstatus;
	int var_amount = 0;
	int index = 1;
	int*** var_indexs;
	int i,j,k;
	Cell** game_board;
	int board_size = board->board_size;

	/*autofill(&board);

	printf("autofilled succeffuly:\n");
	printBoard(board);*/

	game_board = board->current_board;

	/* Create environment */
	error = create_env(&env, &model);
	if(error == 0 || env == NULL || model == NULL)
		return 0;

	/* Add variables */
	var_indexs = allocate_var_indexs(board_size);
	for(i = 0; i < board_size; i++){
		for(j = 0; j < board_size; j++){
			if(game_board[i][j].value == 0){
				game_board[i][j].options = generate_options(board,i,j,0);
				game_board[i][j].is_options_on = 1;
				/*printf("num options for <%d,%d>: %d\n",j+1,i+1,game_board[i][j].options[0]);*/
				if(game_board[i][j].options[0] == 0){
					/*Cell with no valid value found, so there is no solution*/
					destroy_var_indexs(var_indexs, board_size);
					GRBfreemodel(model);
					GRBfreeenv(env);
					return 0;
				}
				for(k = 1; k <= game_board[i][j].options[0]; k++){
					var_indexs[i][j][game_board[i][j].options[k] - 1] = index++;
					/*printf("  Filled val_index %d,%d,%d with %d\n",j+1,i+1,(game_board[i][j].options[k] - 1), index - 1);*/
				}
				var_amount += game_board[i][j].options[0];
			}
		}
	}


	obj = (double*) malloc(var_amount * sizeof(double));
	vtype = (char*) malloc(var_amount * sizeof(char));
	ind = (int*) malloc(board_size * sizeof(int));
	val = (double*) malloc(board_size * sizeof(double));
	sol = (double*) malloc(var_amount * sizeof(double));

	if( !obj || !vtype || !ind || !val || !sol){
		printf(MALLOC_ERROR);
		exit(0);
	}

	error = add_variables(&env, &model, var_amount, &obj, &vtype);
	if(error == 0){
		free_resorces(env, model, vtype, var_indexs, obj, ind, val, sol, board_size);
		return 0;
	}

	error = add_constraints(board, &env, &model, var_amount, &ind, &val, var_indexs);
	if(error == 0){
		free_resorces(env, model, vtype, var_indexs, obj, ind, val, sol, board_size);
		return 0;
	}

	/* Optimize model */
	error = GRBoptimize(model);
	if (error) {
		printf("ERROR %d GRBoptimize(): %s\n", error, GRBgeterrormsg(env));
		free_resorces(env, model, vtype, var_indexs, obj, ind, val, sol, board_size);
		return 0;
	}

	/* Write model to 'mip1.lp' */
	error = GRBwrite(model, "integerLinearPrograming.lp");
	if (error) {
		printf("ERROR %d GRBwrite(): %s\n", error, GRBgeterrormsg(env));
		free_resorces(env, model, vtype, var_indexs, obj, ind, val, sol, board_size);
		return 0;
	}

	/* Get solution information */
	error = GRBgetintattr(model, GRB_INT_ATTR_STATUS, &optimstatus);
	if (error) {
		printf("ERROR %d GRBgetintattr(): %s\n", error, GRBgeterrormsg(env));
		free_resorces(env, model, vtype, var_indexs, obj, ind, val, sol, board_size);
		return 0;
	}

	if(optimstatus == GRB_OPTIMAL){
		if(save_solution == 1){
		/* get the solution - the assignment to each variable */
			error = GRBgetdblattrarray(model, GRB_DBL_ATTR_X, 0, var_amount, sol);
			if (error) {
				printf("ERROR %d GRBgetdblattrarray(): %s\n", error, GRBgeterrormsg(env));
				free_resorces(env, model, vtype, var_indexs, obj, ind, val, sol, board_size);
				return 0;
			  }
			save_sol_to_board(board,sol,var_indexs);
			printf("saved the solution to board\n"); /*need to get earased go*/
		}
		free_resorces(env, model, vtype, var_indexs, obj, ind, val, sol, board_size);
		return 1;
	}
	else{ /*no solutions was found*/
		free_resorces(env, model, vtype, var_indexs, obj, ind, val, sol, board_size);
		return -1;
	}
}

