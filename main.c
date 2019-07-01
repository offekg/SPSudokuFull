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

int game_runner(){
	Command* command;
	char userInput[MAX_COMMAND_SIZE] = { 0 };
	Board* board;
	board = create_blank_board(9,3,3);
	generate_user_board(board);
	printBoard(board,0);

	while(1){
		if (fgets(userInput, MAX_COMMAND_SIZE, stdin) == NULL) {
			if (ferror(stdin)) {
				printf("Error: fgets has failed\n");
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
	SP_BUFF_SET();
	srand(seed);

	Command* command;
	char userInput[MAX_COMMAND_SIZE] = { 0 };
	Board* board;
	board = create_blank_board(9,3,3);
	generate_user_board(board);
	printBoard(board,0);

	while(1){
		if (fgets(userInput, MAX_COMMAND_SIZE, stdin) == NULL) {
			if (ferror(stdin)) {
				printf("Error: fgets has failed\n");
			}
			return 0;
		}

		command = parse_command(userInput);
		if (!command) {
			/*
			 * Could not parse a legal command, skipping.
			 */
			continue;
		}
		execute_command(command, board);
	}
	destroyBoard(board);
	return 0;
}

