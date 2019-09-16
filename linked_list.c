#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

#include "board_utils.h"
#include "linked_list.h"
#include "stack.h"


/*
 * Function that creates an instance of a MovesList.
 * Returns a pointer to the node.
 */
MovesList* initialize_move_list() {
	MovesList* list;

	if((list = (MovesList*) malloc(sizeof(MovesList))) == NULL){
		fprintf(stderr,"%s",MALLOC_ERROR);
		exit(0);
	}
	list->length = 0;
	list->top = NULL;

	return list;
}

/*
 * Receives moves and safely frees all related memory.
 */
void destroy_move_list(MovesList* moves) {
	Node* node = moves->top;

	while (node) {
		node = moves->top->next;
		free(moves->top);
		moves->top = node;
		moves->length = moves->length - 1;
	}
}

/*
 * Function that adds a new move to the move_list
 *
 * 		row: The row number of the changed cell.
 *		col: The col number of the changed cell.
 *		previous_val: The previous value of the changed cell.
 *		new_val: The new value of the changed cell.
 * 		next: The next move that the player made after the current move.
 * 		previous: The previous move that the player made before the current move.
 */
void add_move(MovesList* moves, int row, int col, int previous_val, int new_val) {
	Node* node;

	if((node = (Node*) malloc(sizeof(Node))) == NULL){
		fprintf(stderr,"%s",MALLOC_ERROR);
		exit(0);
	}

	node->row = row;
	node->col = col;
	node->previous_val = previous_val;
	node->new_val = new_val;

	if (moves->top != NULL) {
		node->next = moves->top;
		moves->top->previous = node;
		moves->top = node;
		node->previous = NULL;
	} else {
		node->previous = NULL;
		node->next = NULL;
		moves->top = node;
	}

	moves->length += 1;
}

/*
 * Function that creates an instance of a TurnsList.
 * Returns a pointer to the TurnsList.
 */
TurnsList* initialize_turn_list() {
	TurnsList* list;

	if((list = (TurnsList*) malloc(sizeof(TurnsList))) == NULL){
		fprintf(stderr,"%s",MALLOC_ERROR);
		exit(0);
	}

	list->length = 0;
	list->top = NULL;
	list->current_move = NULL;
	list->position_in_list = 0;

	return list;
}

/*
 * Receives a turn and safely frees all related memory.
 */
void destroy_turn_list(TurnsList* turns) {
	TurnNode* node = turns->top;

	while (node) {
		node = turns->top->next;
		destroy_move_list(turns->top->current_changes);
		free(turns->top);
		turns->top = node;
		turns->length = turns->length - 1;
	}
}

/*
 * Function that adds a new move to the turn_list
 *
 * 		turns: The list of turns we want to update
 * 		moves: The moves the player made in the current turn.
 */
void add_turn(TurnsList* turns, MovesList* moves) {
	TurnNode* node;

	if((node = (TurnNode*) malloc(sizeof(TurnNode))) == NULL){
		fprintf(stderr,"%s",MALLOC_ERROR);
		exit(0);
	}

	/*
	 * In case the current turn is after an undo.
	 */
	remove_turns_after_current(turns);

	node->current_changes = moves;
	if (turns->length != 0) {
		node->previous = turns->current_move;
		turns->current_move->next = node;
	} else {
		node->previous = NULL;
		turns->top = node;
	}
	node->next = NULL;
	turns->current_move = node;
	turns->length += 1;
	turns->position_in_list += 1;
}

/*
 * Removes the turns after the current node.
 * Needs to get the current node.
 */
void remove_turns_after_current(TurnsList* turns) {
	TurnNode* node, *tmp;
	TurnNode* turn = turns->current_move;

	if (!turn)
		return;

	node = turn->next;
	while (node) {
		tmp = node->next;
		destroy_move_list(node->current_changes);
		free(node);
		turns->length = turns->length - 1;
		node = tmp;
		turns->position_in_list -=1;
	}
}
