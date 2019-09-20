#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <time.h>

#include "game.h"
#include "parser.h"
#include "solver.h"
#include "SPBufferset.h"
#include "main_aux.h"
#include "board_utils.h"

#define MAX_COMMAND_SIZE 256

enum game_mode current_mode = INIT_MODE;
int mark_errors = 1;



int main(){
	Command* command;
	char userInput[MAX_COMMAND_SIZE+2] = { 0 };

	srand(time(0));
	SP_BUFF_SET();
	opening_message();

	while(1){
		printf("\nPlease enter a command:\n");
		if (fgets(userInput, MAX_COMMAND_SIZE+3, stdin) == NULL) {
			if (ferror(stdin)) {
				printf("Error: fgets has failed\n");
				exit(0);
			}
			printf("Exiting...\n");
			exit(0);
		}
		if (userInput[MAX_COMMAND_SIZE+1] != 0){
			printf("Error: Invalid Command - Entered more then 256 characters!\n");
			clear_input_line();
			userInput[MAX_COMMAND_SIZE+1] = 0;
			continue;
		}

		command = parse_command(userInput);
		if (!command) {
			/*
			 * Could not parse a legal command, skipping.
			 */
			continue;
		}
		execute_command(command);
		destroy_command_object(command);
	}
	return 0;
}


