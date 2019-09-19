#ifndef GAME_H_
#define GAME_H_

#include "parser.h"
#include "board_utils.h"
#include "linked_list.h"

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
 * Function receives a board, a cell's row&col and a value;
 * The function inserts the value in the cell, without any checks.
 * num_empty_cells is updated appropriately.
 */
void set_value_simple(Board* b, int row, int col, int inserted_val);


/*
 *Recieves given command from user, and implements it appropriately.
 */
void execute_command(Command* command);


/*
 * Function checks if the given board has no more empty cells.
 * If board is full - return 1; otherwise return 0;
 * If board is full the function checks if the board has errors:
 *     If there aren't, then the game mode is switched to INIT,
 *     and the game board is destroyed.
 *     A message is printed acourdingly.
 * to_print: if it is 1, messeges are printed. otherwise it isn't.
 */
int check_full_board(Board* b, int to_print);


int autofill(Board** board);

/*
 * Exits gracfully from game
 */
void exit_game(Board* board);


#endif
