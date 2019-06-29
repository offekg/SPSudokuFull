
#ifndef PARSER_H_
#define PARSER_H_

enum command_id {
	INVALID_COMMAND, SET, HINT, VALIDATE, RESTART, EXIT
};

typedef struct command_t{
	int id;
	int params[3];
} Command;


int get_fixed_cells();
Command* parse_command(char userInput[]);


#endif
