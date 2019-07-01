#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#include "main_aux.h"
#include "parser.h"
#include "game.h"

#define DELIMITER " \t\r\n"
#define MALLOC_ERROR "Error: malloc has failed\n"


/*
 * Scans input from user for number of wanted fixed cells in the board.
 * returns it when the input is legal.
 */
int get_fixed_cells(Board* board) {
	int fixedCells;
	if (scanf("%d", &fixedCells) != 1) {
		checkEOF(board);
	}
	while (fixedCells < 0 || fixedCells > 80){
		printf("Error: invalid number of cells to fill (should be between 0 and 80)\n");
		printf("Please enter the number of cells to fill [0-80]:\n");
		if (scanf("%d", &fixedCells) != 1) {
			checkEOF(board);
		}

	}
	return fixedCells;
}

const char* get_command_name(int cmd_id) {
	static char* names[] = { "invalid_command", "set", "hint", "validate", "restart", "exit" };
	if (cmd_id < INVALID_COMMAND || cmd_id > EXIT) {
		return 0;
	} else {
		return names[cmd_id];
	}
}

int get_command_id(char *type) {
	int cmd_id;
	if (!type || type == '\0')
		return -1;
	for (cmd_id = INVALID_COMMAND; cmd_id <= EXIT; cmd_id++) {
		if (!strcmp(type, get_command_name(cmd_id)))
			return cmd_id;
	}
	return -1;
}

/*
 *Creates a Command struct out of the user input.
 */
Command* create_new_command_object(int cmd_id, int params[3], int param_counter) {
	int i;
	Command* cmd = (Command*) malloc(sizeof(Command));
	if (cmd == NULL) {
		printf(MALLOC_ERROR);
		exit(EXIT_FAILURE);
	}

	cmd->id = cmd_id;
	cmd->param_counter = param_counter;

	for (i = 0; i < 3; i++) {
		cmd->params[i] = params[i];
	}
	return cmd;
}

/*
 * Gets given input from the user.
 * Parses it to a specific command, including the paramaters.
 */
Command* parse_command(char userInput[]) {
	int cmd_id;
	int param_counter = 0;
	int params[3] = { 0 };

	char *token = strtok(userInput, DELIMITER);

	if (feof(stdin) && !token) {
		cmd_id = EXIT;
		return create_new_command_object(cmd_id, params, 0);
	}

	if (!token) {
		/*
		 * Failed to parse any input from the user, hence continuing.
		 */
		return NULL;
	}
	cmd_id = get_command_id(token);

	/*
	 * Populating the param list.(No need to check more then 3 params)
	 */
	token = strtok(NULL, DELIMITER);
	while(token && param_counter < 3) {
		params[param_counter] = atoi(token);
		/* We were directed to assume that X Y Z are valid integers so no need to check validity. */
		param_counter++;
		token = strtok(NULL, DELIMITER);
	}

	return create_new_command_object(cmd_id, params, param_counter);
}
