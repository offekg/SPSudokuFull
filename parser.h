
#ifndef PARSER_H_
#define PARSER_H_


enum command_id {
	INVALID_COMMAND, SET, HINT, VALIDATE, RESTART, EXIT
};

/*
 * Struct: Command
 * 		Used to represent a given user command.
 *
 * 		id: the identification of which command this is.
 * 		params: the paramaters that were given by the user for this command.
 */
typedef struct command_t{
	int id;
	int params[3];
} Command;

/*
 * Scans input from user for number of wanted fixed cells in the board.
 * returns it when the input is legal.
 */
int get_fixed_cells();

/*
 * Gets given input from the user.
 * Parses it to a specific command, including the paramaters.
 */
Command* parse_command(char userInput[]);


#endif
