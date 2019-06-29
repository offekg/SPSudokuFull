#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

#include "game.h"
#include "parser.h"
#include "solver.h"
#include "SPBufferset.h"

#define MAX_COMMAND_SIZE 1026


void board_test(){
	int i, j;
	Board* b;
	Cell* c = (Cell*) malloc(sizeof(Cell));
	createCell(c,5);
	printf("Cell's value: %d, Cell's fixed: %d.\n",c->value,c->isFixed);
	c->value += 3;
	c->isFixed = 1;
	printf("after change: Cell's value: %d, Cell's fixed: %d.\n",c->value,c->isFixed);
	b = createBoard(9,3,3);
	for(i = 0; i < 9; i++){
		for(j = 0; j < 9; j++){
			printf("%d ",(b->current_board[i][j]).value);
			(b->current_board[i][j]).value = i+j;
		}
		printf("\n");
	}
	printf("\n\n");
	for(i = 0; i < 9; i++){
		for(j = 0; j < 9; j++){
			printf("%d ",(b->current_board[i][j]).value);
		}
		printf("\n");
	}
	destroyBoard(b);
}

int main(){
	SP_BUFF_SET();
	printf("Starting");
	board_test();

	return 0;
}


int real_main(){
	Command* command;
	char userInput[MAX_COMMAND_SIZE] = { 0 };
	Board* solvedBoard = generateRandomBoard();
	Board* userBoard = create_user_board(solvedBoard);

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
		execute_command(command, userBoard);
	}

	return 0;
}
