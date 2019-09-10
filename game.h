#ifndef GAME_H_
#define GAME_H_

#include "parser.h"


typedef enum game_mode {
	INIT_MODE, EDIT_MODE, SOLVE_MODE
}game_mode;

extern game_mode current_mode;
extern int mark_errors;




/*
 * Prints to the prompt the opening greeting and initial instructions to the user.
 */
void opening_message();

/*
 *Recieves given command from user, and implements it appropriately.
 */
void execute_command(Command* command);

/*
 * Exits gracfully from game
 */
void exit_game(Board* board);


#endif
