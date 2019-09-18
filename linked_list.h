
#ifndef LINKED_LIST_H_
#define LINKED_LIST_H_

#define EMPTY 0


/*
 * Structure: MoveNode
 * 		Used to represent a node in the players move list.
 *
 *		row: The row number of the changed cell.
 *		col: The col number of the changed cell.
 *		previous_val: The previous value of the changed cell.
 *		new_val: The new value of the changed cell.
 * 		next: The next move that the player made after the current move.
 * 		previous: The previous move that the player made before the current move.
 */
typedef struct node{
	int row;
	int col;
	int previous_val;
	int new_val;
	struct node* next;
	struct node* previous;
} Node;


/*
 * Structure: movesList
 *		A structure used to represent a linked list, which will be the moves list for a current turn.
 *
 * 		top : a pointer to the first element of the list
 * 		length : an integer representing the length of the list
 */
typedef struct {
	Node* top;
	int length;
} MovesList;


/*
 * Structure: turnNode
 * 		A structure used to represent a node in the turns linked list
s
 */
typedef struct turn_node {
	MovesList* current_changes;
	struct turn_node* next;
	struct turn_node* previous;
} TurnNode;


/*
 * Structure: TurnList
 * 		A structure used to represent a TurnsList.
 *
 * 		top : a pointer to the first element of the list
 * 		current_move : a pointer to the current move the user is at.
 * 		length : an integer representing the length of the list
 * 		position_in_list: Position of the current move in the list, 0 is before any moves.
 */
typedef struct turn_list {
	TurnNode* top;
	TurnNode* current_move;
	int length;
	int position_in_list;
} TurnsList;


/*
 * Function that creates an instance of a MovesList.
 * Returns a pointer to the node.
 */
MovesList* initialize_move_list();

/*
 * Receives a move and safely frees all related memory.
 */
void destroy_move_list(MovesList* moves);

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
void add_move(MovesList* moves, int row, int col, int previous_val, int new_val);

/*
 * Function that creates an instance of a TurnsList.
 * Returns a pointer to the node.
 */
TurnsList* initialize_turn_list();

/*
 * Receives a turn and safely frees all related memory.
 */
void destroy_turn_list(TurnsList* turns);

/*
 * Function that adds a new move to the turn_list
 *
 * 		turns: The list of turns we want to update
 * 		moves: The moves the player made in the current turn.
 */
void add_turn(TurnsList* turns, MovesList* moves);

/*
 * Removes the turns after the current node.
 * Needs to get the current node.
 */
void remove_turns_after_current(TurnsList* turns);

#endif /* LINKED_LIST_H_ */
