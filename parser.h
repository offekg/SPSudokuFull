/*
 * The "parser" module is in charge of parsing the diffrent inputs from the user,
 * to command strucures that the game knows how to handle.
 * Initial checks on simple validity of the user input are done here.
 */

#ifndef PARSER_H_
#define PARSER_H_


enum command_id {
	INVALID_COMMAND, SOLVE, EDIT, MARK_ERRORS, PRINT_BOARD,
	SET, VALIDATE, GENERATE, UNDO, REDO, SAVE, HINT,
	NUM_SOLUTIONS, AUTOFILL, RESET, EXIT
};

/*
 * Struct: Command
 * 		Used to represent a given user command.
 *
 * 		id: the identification of which command this is.
 * 		params: the integer paramaters that were given by the user for this command.
 * 		path_param: a string variable for commands containing a path paramater.
 */
typedef struct command_t{
	int id;
	int params[3];
	char* path_param;
	int param_counter;
} Command;

/*
 * Scans input from user for number of wanted fixed cells in the board.
 * returns it when the input is legal.
 */
int get_fixed_cells();

/*
 * Gets a string and returns 1 if the string has only digits (0-9) in it.
 * Otherwise - returns 0.
 */
int is_string_a_int(char** str, int length);

/*
 * Gets given input from the user.
 * Parses it to a specific command, including the paramaters.
 */
Command* parse_command(char userInput[]);

/*
 * safely destroys a Command struct, freeing all used resources.
 */
void destroy_command_object(Command* cmd);

#endif
