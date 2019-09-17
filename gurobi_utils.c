/*
 * gurobi_utils.c
 *
 *  Created on: Sep 15, 2019
 *      Author: offek
 */

/* Copyright 2013, Gurobi Optimization, Inc. */

#include <stdlib.h>
#include <stdio.h>
#include "gurobi_c.h"
#include "board_utils.h"

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
		  return 0;
	  }

	  /* Create an empty model named "mip1" */
	  error = GRBnewmodel(*env, model, "integerLinearPrograming", 0, NULL, NULL, NULL, NULL, NULL);
	  if (error) {
		  printf("ERROR: %d GRBnewmodel(): %s\n", error, GRBgeterrormsg(*env));
		  return 0;
	  }
	  return 1;
}

int add_variables(GRBenv** env, GRBmodel** model, int var_amount, int* var_indexs,
							char** vtype, int board_size){
	double* obj = (double*) malloc(var_amount * sizeof(double));
	int i, j, k;
	int error;
	/* coefficients - for x,y,z (cells 0,1,2 in "obj") */
	  obj[0] = 1; obj[1] = 3; obj[2] = 2;

	  /* variable types - for x,y,z (cells 0,1,2 in "vtype") */
	  /* other options: GRB_INTEGER, GRB_CONTINUOUS */
	  vtype[0] = GRB_BINARY; vtype[1] = GRB_BINARY; vtype[2] = GRB_BINARY;

	  /* add variables to model */
	  error = GRBaddvars(model, 3, 0, NULL, NULL, NULL, obj, NULL, NULL, vtype, NULL);
	  if (error) {
		  printf("ERROR %d GRBaddvars(): %s\n", error, GRBgeterrormsg(env));
		  return -1;
	  }

	return 0;
}

int find_ILP_solution(Board* board){
	GRBenv   *env   = NULL;
	GRBmodel *model = NULL;
	double*   sol;
	int*      ind;
	double*   val;
	double*   obj;
	char*     vtype;
	int       optimstatus;
	double    objval;
	int var_amount = 0;
	int* var_indexs;
	int i,j,k;
	Cell** game_board = board->current_board;
	int board_size = board->board_size;
	/*Board* b_copy = copy_Board(board);  maybe do this in game.c before call */

	autofill(board); /* maybe do this in game.c before call */
	/* Create environment - log file is mip1.log */

	if(create_env(&env, &model) == 0)
		return 0;

	/* Add variables */
	for(i = 0; i < board_size; i++){
		for(j = 0; j < board_size; j++){
			if(game_board[i][j].value == 0){
				game_board[i][j].options = generate_options(board,i,j,0);
				var_amount += game_board[i][j].options[0];
			}
		}
	}


	obj = (double*) malloc(var_amount * sizeof(double));


	return 1;
}

/* This example formulates and solves the following simple MIP model:

     maximize    x + 3 y + 2 z
     subject to  x + 2 y + 3 z <= 5
                 x +   y       >= 1
     x, y, z binary
*/

int example(Board* board)
{
  GRBenv   *env   = NULL;
  GRBmodel *model = NULL;
  int       error = 0;
  double    sol[3];
  int       ind[3];
  double    val[3];
  double    obj[3];
  char      vtype[3];
  int       optimstatus;
  double    objval;

  /* Create environment - log file is mip1.log */
  if(create_env(&env, &model) == 0)
	  return 0;

  /* Add variables */



  /* Change objective sense to maximization */
  error = GRBsetintattr(model, GRB_INT_ATTR_MODELSENSE, GRB_MAXIMIZE);
  if (error) {
	  printf("ERROR %d GRBsetintattr(): %s\n", error, GRBgeterrormsg(env));
	  return -1;
  }

  /* update the model - to integrate new variables */

  error = GRBupdatemodel(model);
  if (error) {
	  printf("ERROR %d GRBupdatemodel(): %s\n", error, GRBgeterrormsg(env));
	  return -1;
  }


  /* First constraint: x + 2 y + 3 z <= 5 */

  /* variables x,y,z (0,1,2) */
  ind[0] = 0; ind[1] = 1; ind[2] = 2;
  /* coefficients (according to variables in "ind") */
  val[0] = 1; val[1] = 2; val[2] = 3;

  /* add constraint to model - note size 3 + operator GRB_LESS_EQUAL */
  /* -- equation value (5.0) + unique constraint name */
  error = GRBaddconstr(model, 3, ind, val, GRB_LESS_EQUAL, 5, "c0");
  if (error) {
	  printf("ERROR %d 1st GRBaddconstr(): %s\n", error, GRBgeterrormsg(env));
	  return -1;
  }

  /* Second constraint: x + y >= 1 */
  ind[0] = 0; ind[1] = 1;
  val[0] = 1; val[1] = 1;

  /* add constraint to model - note size 2 + operator GRB_GREATER_EQUAL */
  /* -- equation value (1.0) + unique constraint name */
  error = GRBaddconstr(model, 2, ind, val, GRB_GREATER_EQUAL, 1.0, "c1");
  if (error) {
	  printf("ERROR %d 2nd GRBaddconstr(): %s\n", error, GRBgeterrormsg(env));
	  return -1;
  }

  /* Optimize model - need to call this before calculation */
  error = GRBoptimize(model);
  if (error) {
	  printf("ERROR %d GRBoptimize(): %s\n", error, GRBgeterrormsg(env));
	  return -1;
  }

  /* Write model to 'mip1.lp' - this is not necessary but very helpful */
  error = GRBwrite(model, "mip1.lp");
  if (error) {
	  printf("ERROR %d GRBwrite(): %s\n", error, GRBgeterrormsg(env));
	  return -1;
  }

  /* Get solution information */

  error = GRBgetintattr(model, GRB_INT_ATTR_STATUS, &optimstatus);
  if (error) {
	  printf("ERROR %d GRBgetintattr(): %s\n", error, GRBgeterrormsg(env));
	  return -1;
  }

  /* get the objective -- the optimal result of the function */
  error = GRBgetdblattr(model, GRB_DBL_ATTR_OBJVAL, &objval);
  if (error) {
	  printf("ERROR %d GRBgettdblattr(): %s\n", error, GRBgeterrormsg(env));
	  return -1;
  }

  /* get the solution - the assignment to each variable */
  /* 3-- number of variables, the size of "sol" should match */
  error = GRBgetdblattrarray(model, GRB_DBL_ATTR_X, 0, 3, sol);
  if (error) {
	  printf("ERROR %d GRBgetdblattrarray(): %s\n", error, GRBgeterrormsg(env));
	  return -1;
  }

  /* print results */
  printf("\nOptimization complete\n");

  /* solution found */
  if (optimstatus == GRB_OPTIMAL) {
    printf("Optimal objective: %.4e\n", objval);
    printf("  x=%.2f, y=%.2f, z=%.2f\n", sol[0], sol[1], sol[2]);
  }
  /* no solution found */
  else if (optimstatus == GRB_INF_OR_UNBD) {
    printf("Model is infeasible or unbounded\n");
  }
  /* error or calculation stopped */
  else {
    printf("Optimization was stopped early\n");
  }

  /* IMPORTANT !!! - Free model and environment */
  GRBfreemodel(model);
  GRBfreeenv(env);

  return 0;
}

