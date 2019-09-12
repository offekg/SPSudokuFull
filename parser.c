#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>


#include "main_aux.h"
#include "parser.h"
#include "game.h"

#define DELIMITER " \t\r\n"
//#define MALLOC_ERROR "Error: malloc has failed. Now terminating.\n"


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
	static char* names[] = { "invalid_command","solve", "edit", "mark_errors",
			"print_board", "set", "validate", "generate", "undo", "redo", "save",
			"hint","num_solutions", "autofill", "restart", "exit" };
	if (cmd_id < INVALID_COMMAND || cmd_id > EXIT) {
		return 0;
	} else {
		return names[cmd_id];
	}
}

enum command_id get_command_id(char *type) {
	enum command_id cmd_id;
	if (!type || type == '\0')
		return INVALID_COMMAND;
	for (cmd_id = INVALID_COMMAND; cmd_id <= EXIT; cmd_id++) {
		if (!strcmp(type, get_command_name(cmd_id)))
			return cmd_id;
	}
	return INVALID_COMMAND;
}

/*
 *Creates a Command struct out of the user input.
 */
Command* create_new_command_object(int cmd_id, int params[3], int param_counter, char* path_param) {
	int i;
	Command* cmd = (Command*) malloc(sizeof(Command));
	if (cmd == NULL) {
		printf(MALLOC_ERROR);
		exit(EXIT_FAILURE);
	}

	cmd->id = cmd_id;
	cmd->param_counter = param_counter;

	for (i = 0; i < 3; i++)
		cmd->params[i] = params[i];

	if(path_param){
		cmd->path_param = (char*) malloc(sizeof(char) * (strlen(path_param) + 1));
		if (cmd->path_param == NULL) {
				printf(MALLOC_ERROR);
				exit(EXIT_FAILURE);
			}
		strcpy(cmd->path_param,path_param);
	}
	else
		cmd->path_param = NULL;
	//cmd->path_param = *path_param;
	return cmd;
}

void destroy_command_object(Command* cmd){
	if(!cmd)
		return;
	if(cmd->path_param)
		free(cmd->path_param);
	free(cmd);
}

/*
 * Returns the expected number of paramaters for each given command, by it's command_id.
 */
int get_num_params(enum command_id cmd_id){
	switch(cmd_id) {
		case SOLVE:
		case EDIT:
		case MARK_ERRORS:
		case SAVE:
			return 1;
			break;
		case GENERATE:
		case HINT:
			return 2;
			break;
		case SET:
			return 3;
		default:
		    return 0;
		    break;
		}
}

/*
 * Gets a string and returns 1 if the string has only digits (0-9) in it.
 * Otherwise - returns 0.
 */
int is_string_a_int(char** str, int length){
	int i;
	for (i=0;i<length; i++)
		if (!isdigit((*str)[i]))
			return 0;
	return 1;
}

/*
 * Given a command id, returns 1 if the command is available in the current game mode.
 * Otherwise, returns 0.
 */
int check_command_availability(enum command_id cmd_id){
	 switch(cmd_id){
		/* always available: */
		case SOLVE:
		case EDIT:
		case EXIT:
			return 1;
			break;
		/* only in Solve & Edit modes: */
		case PRINT_BOARD:
		case SET:
		case VALIDATE:
		case UNDO:
		case REDO:
		case SAVE:
		case NUM_SOLUTIONS:
		case RESET:
			if(current_mode == INIT_MODE){
				printf("Error: The command is unavailable in the current game mode.\n");
				printf("%s is available only in SOLVE and EDIT modes.\n",get_command_name(cmd_id));
				return 0;
			}
			else
				return 1;
			break;
		/* only in Solve mode: */
		case MARK_ERRORS:
		case HINT:
		case AUTOFILL:
			if(current_mode == SOLVE_MODE)
				return 1;
			else{
				printf("Error: The command is unavailable in the current game mode.\n");
				printf("%s is available only in SOLVE mode.\n",get_command_name(cmd_id));
				return 0;
			}
			break;
		/* only in Edit mode: */
		case GENERATE:
			if(current_mode == EDIT_MODE)
				return 1;
			else{
				printf("Error: The command is unavailable in the current game mode.\n");
				printf("%s is available only in EDIT mode.\n",get_command_name(cmd_id));
				return 0;
			}
			break;
		case INVALID_COMMAND:
			return 0;
			break;
	}
	return 0;
}

/*
 * Gets given raw input of command from the user.
 * Parses it to a specific command (including it's paramaters), creates a command struct and returns it for execution.
 * If encounters an error, prints a relevant message and returns Null.
 */
Command* parse_command(char* userInput) {
	enum command_id cmd_id;
	int param_counter = 0;
	int params[3] = { 0 };
	char* path_param = NULL;
	int expected_num_params;
	int length;
	int param_not_int = 0;
	int first_bad_param = 0;

	char *token = strtok(userInput, DELIMITER);

	if (feof(stdin) && !token) {
		cmd_id = EXIT;
		return create_new_command_object(cmd_id, params, 0,NULL);
	}

	if (!token) {
		/*
		 * Failed to parse any input from the user, hence continuing.
		 */
		return NULL;
	}
	cmd_id = get_command_id(token);
	if(cmd_id == INVALID_COMMAND){
		printf("Error: Invalid Command - No such command exists.\n");
		return NULL;
	}
	if(!check_command_availability(cmd_id))
		return NULL;

	expected_num_params = get_num_params(cmd_id);
	while( (token = strtok(NULL, DELIMITER)) != NULL) {
		if( param_counter >= expected_num_params ){
			printf("Error: Too many paramaters entered.\n");
			printf("You need to enter %d paramaters.\n",expected_num_params);
			return NULL;
		}

		if(cmd_id == EDIT || cmd_id == SOLVE || cmd_id == SAVE)
			//strcpy(path_param,token);
			path_param = token;
		else{
			length = strlen(token);
			if( !is_string_a_int(&token,length) && !param_not_int){
				param_not_int = 1;
				first_bad_param = (param_counter + 1);
			}
			else
				params[param_counter] = atoi(token);
		}
		param_counter++;
	}

	if( (param_counter < expected_num_params) && (cmd_id != EDIT) ){
		printf("Error: Not enough paramaters entered.\n");
		printf("You need to enter %d paramaters.\n",expected_num_params);
		return NULL;
	}

	if(param_not_int){
		printf("Error: Wrong paramater type.\nParamater %d needs to be a positive integer.\n",first_bad_param);
		return NULL;
	}

	return create_new_command_object(cmd_id, params, param_counter,path_param);
}
