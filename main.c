#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <time.h>

#include "game.h"
#include "parser.h"
#include "solver.h"
#include "SPBufferset.h"

#define MAX_COMMAND_SIZE 1026


void board_test(){
	Board* b;
	/*
	Cell* c = (Cell*) malloc(sizeof(Cell));
	createCell(c,5);
	printf("Cell's value: %d, Cell's fixed: %d.\n",c->value,c->isFixed);
	c->value += 3;
	c->isFixed = 1;
	printf("after change: Cell's value: %d, Cell's fixed: %d.\n",c->value,c->isFixed);
	destroyCell(c);

	b2 = createBoard(6,2,3);
	printf("created b2\n");
	for(i = 0; i < 6; i++){
		for(j = 0; j < 6; j++){
			(b2->current_board[i][j]).value = rand() % 7;
			(b2->current_board[i][j]).isFixed = rand() % 2;
		}
	}
	printBoard(b2, 0);
	destroyBoard(b2);*/

	b = create_blank_board(9,3,3);
	generate_user_board(b);
	printBoard(b,1);
	printBoard(b,0);
	/*for(i = 0; i < 9; i++){
		for(j = 0; j < 9; j++){
			(b->current_board[i][j]).value = i;
		}
	}
	printBoard(b, 0);

	printf("\n\n");
	for(i = 0; i < 9; i++){
		for(j = 0; j < 9; j++){
			(b->current_board[i][j]).value = rand() % 10;
			(b->current_board[i][j]).isFixed = rand() % 2;
		}
	}
	printBoard(b, 0);
	b->current_board[5][3].value = 8;
	b->current_board[1][7].value = 1;
	b->current_board[8][6].value = 2;
	b->current_board[8][7].value = 8;
	b->current_board[8][8].value = 5;
	b->current_board[0][4].value = 1;
	printf("backtrack result: %d\n",backtracking_solution(b, 0));
	printBoard(b, 0);*/

	b->current_board[7][1].value = 5;
	printf("validating\n");
	validate(b);
	printBoard(b,1);
	printBoard(b,0);
	destroyBoard(b);
	printf("Good bye");

	/*
	destroyCell(c);
	*/

}

int real_main(){
	Command* command;
	char userInput[MAX_COMMAND_SIZE] = { 0 };
	Board* board;
	board = create_blank_board(9,3,3);
	generate_user_board(board);
	printBoard(board,0);
	/* Board* solvedBoard = generateRandomBoard();
	Board* userBoard = create_user_board(solvedBoard); */


	while(1){
		if (fgets(userInput, MAX_COMMAND_SIZE, stdin) == NULL) {
			if (ferror(stdin)) {
				printf("Error: fgets has failed\n");
				/*
				 * Not sure if this is needed but it seems like something we should check.
				 */
			}
			return 0;
		}


		command = parse_command(userInput);
		if (!command) {
			/*
			 * Could not parse a legible command, skipping.
			 */
			continue;
		}
		execute_command(command, board);
	}
	destroyBoard(board);
	return 0;
}


int main(int argc, char *argv[]){
	int seed = atoi(argv[1]);
	printf("%d", argc);
	SP_BUFF_SET();
	srand(seed);
	printf("Starting");
	return real_main();
}

