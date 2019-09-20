/*
 * The "linked_list" module contains the to-way linked list structure and relevant use functions,
 * that are used for saving the users moves on the board.
 * The struct itself is saved for each Board structure.
 */

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
	Node* node;

	if(moves != NULL){
		node = moves->top;

		while (node) {
			node = moves->top->next;
			free(moves->top);
			moves->top = node;
			moves->length = moves->length - 1;
		}
		free(moves);
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
	TurnNode* node;
	if(turns != NULL){
		node = turns->top;

		while (node) {
			node = turns->top->next;
			destroy_move_list(turns->top->current_changes);
			free(turns->top);
			turns->top = node;
			turns->length = turns->length - 1;
		}
		free(turns);
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
	if(turns->length > turns->position_in_list){
		remove_turns_after_current(turns);
	}

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
	turns->position_in_list = turns->length;
}

/*
 * Removes the turns after the current node.
 * Needs to get the current node.
 */
void remove_turns_after_current(TurnsList* turns) {
	TurnNode* node, *tmp;
	TurnNode* turn = turns->current_move;

	if (turns->current_move == NULL && turns->top == NULL){
		return;
	}

	if(turn == NULL)
		node = turns->top;
	else
		node = turn->next;
	while (node != NULL) {
		tmp = node->next;
		destroy_move_list(node->current_changes);
		free(node);
		turns->length -= 1;
		node = tmp;
		/*print_moves(node->current_changes);*/
		if(turns->position_in_list != 0)
			turns->position_in_list -= 1;
	}
	if(turn != NULL)
		turn->next = NULL;
	else
		turns->top = NULL;
}

/*
 * Returns a copy of a given MovesList.
 */
MovesList* copy_moves_list(MovesList* moves){
	MovesList* copy_moves;
	Node* temp;

	if(moves == NULL)
		return NULL;

	copy_moves = initialize_move_list();
	temp = moves->top;
	while(temp){
		add_move(copy_moves,temp->row,temp->col,temp->previous_val,temp->new_val);
		temp = temp->next;
	}
	return copy_moves;
}

/*
 * Returns a copy of the given TurnsList.
 */
TurnsList* copy_turns_list(TurnsList* turns){
	TurnsList* copy_list = initialize_turn_list();
	TurnNode* turn_node;
	MovesList* copy_moves;
	int pos = 1;

	if(turns == NULL)
		return NULL;

	copy_list = initialize_turn_list();
	if((turn_node = turns->top) != NULL){
		while(turn_node){
			copy_moves = copy_moves_list(turn_node->current_changes);
			add_turn(copy_list,copy_moves);
			turn_node = turn_node->next;
		}

		copy_list->position_in_list = turns->position_in_list;
		if(copy_list->position_in_list == 0)
			copy_list->current_move = NULL;
		else{
			copy_list->current_move = copy_list->top;
			while(pos != copy_list->position_in_list){
				copy_list->current_move = copy_list->current_move->next;
				pos++;
			}
		}
		copy_list->length = turns->length;
	}
	/*print_turns(copy_list);*/
	return copy_list;
}

void print_moves(MovesList* moves){
	Node* temp;
	if(moves == NULL || moves->top == NULL){
		printf("The moves node is empty.\n");
	}
	temp = moves->top;
	printf("  Moves list length %d:\n",moves->length);
	while(temp){
		printf("      cell <%d,%d> new val: %d, old val: %d\n",temp->col+1,temp->row+1,temp->new_val,temp->previous_val);
		temp = temp->next;
	}
}

void print_turns(TurnsList* turns){
	TurnNode* turn_node;
	if(turns == NULL || turns->top == NULL){
		printf("Turns list is empty and length is %d\n",turns->length);
		return;
	}
	turn_node = turns->top;
	printf("Printing current Turns List with length %d:\n",turns->length);
	while(turn_node){
		print_moves(turn_node->current_changes);
		turn_node = turn_node->next;
	}
	printf("the current move was:");
	if(turns->current_move != NULL)
		print_moves(turns->current_move->current_changes);
	else
		printf(" NULL\n");
	printf("position in the list is: %d\n",turns->position_in_list);

}
