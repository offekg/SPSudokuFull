#ifndef MAIN_AUX_H_
#define MAIN_AUX_H_

#include "game.h"
#include "board_utils.h"

/*
 * Checks if reached EOF. if so - exists gracfully, as if given "exit" command.
 */
void checkEOF(Board* board);

/*
 * In case of a command larger then the max length allowed in a single line,
 * the function clears the rest of the given line.
 */
void clear_input_line();

#endif
