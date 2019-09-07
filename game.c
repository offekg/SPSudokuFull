#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

#include "game.h"
#include "parser.h"
#include "solver.h"

#define MALLOC_ERROR "Error: malloc has failed"

/*
 * Prints to the prompt the opening greeting and initial instructions to the user.
 */
void opening_message(){
	printf("WELCOME TO THE GAME SUDOKU!\n\n\n");
	printf("Plesae choose if you would like to edit a game board or solve an existing one.\n");
	printf("Have fun!\n");
}


/*
 * Gets a pointer to a cell, and initializes it with given value.
 * fixed == 1 means cell is fixed; fixed == 0 means cell is not fixed
 * Initializes as non fixed cell.
 */
void createCell(Cell* cell,int val){
	cell->value = val;
	cell->isFixed = 0;
}

/*
 * Receives dimensions of the wanted board and the blocks in the board.
 * Returns a pointer to a Board struct, with the current game board and solution board set to default (all zeros).
 */
Board* create_blank_board(int blockCols, int blockRows){
	int i, j;
	Board* board;
	Cell** solution;
	Cell** current;

	if((board = (Board*) malloc(sizeof(Board))) == NULL){
		fprintf(stderr,"%s",MALLOC_ERROR);
		return NULL;
	}
	board->block_rows = blockRows;
	board->block_cols = blockCols;
	board->board_size = blockCols*blockRows;
	board->num_empty_cells_current = board->board_size*board->board_size;
	board->num_empty_cells_solution = board->board_size*board->board_size;


	if((solution = (Cell**) malloc(board->board_size*sizeof(Cell*))) == NULL){
			fprintf(stderr,"%s",MALLOC_ERROR);
			free(board);
			return NULL;
		}
	if((current = (Cell**) malloc(board->board_size*sizeof(Cell*))) == NULL){
		fprintf(stderr,"%s",MALLOC_ERROR);
		free(board);
		return NULL;
	}
	for(i = 0; i < board->board_size; i++){
		solution[i] = (Cell*) malloc(board->board_size*sizeof(Cell));
		current[i] = (Cell*) malloc(board->board_size*sizeof(Cell));
		for(j = 0; j < board->board_size; j++){
			createCell(&solution[i][j],0);
			createCell(&current[i][j],0);
		}
	}
	board->solution = solution;
	board->current_board = current;

	return board;
}

/*
 * Destroys properly a given game board, freeing all allocated resources.
 */
void destroy_game_board(Cell** board, int size){
	int i;
	for(i = 0; i < size; i++){
		free(board[i]);
	}
	free(board);
}

/*
 * Destroys properly a given Board, freeing all allocated resources.
 */
void destroyBoard(Board* b){
	destroy_game_board(b->current_board, b->board_size);
	destroy_game_board(b->solution, b->board_size);
	free(b);
}


/*
 * Prints a single cell, acording to the sudoku board format.
 */
void printCell(Cell* c){
	if(c->isFixed == 0 && c->value != 0)
		printf(" %2d ",c->value);
	else
		if(c->value != 0)
			printf(" %2d.",c->value);
		else
			printf("    ");
}


/*
 * Prints the given board.
 * if type == 1: prints the board's known solution.
 * otherwise, prints board's current state board.
 */
void printBoard(Board* b, int type){
	int i, j;
	Cell** board;
	char* sep_row;
	int total_row_length = (4* b->board_size) + b->block_rows + 1;
	//int total_row_length = (b->board_size * 3) + ((b->board_size / b->block_cols) * 2) + 1;

	if(type == 1)
		board = b->solution;
	else
		board = b->current_board;

	sep_row = (char*) malloc((total_row_length + 2)*sizeof(char));
	for( i = 0; i < total_row_length; i++ ){
		sep_row[i] ='-';
	}
	sep_row[total_row_length] = '\n';
	sep_row[total_row_length + 1] = '\0';


	for( i = 0; i < b->board_size; i++){
		if( i % b->block_rows == 0 )
			printf("%s",sep_row);
		for( j = 0; j < b->board_size; j++){
			if( j % b->block_cols == 0 )
				printf("|");
			printCell(&(board[i][j]));
		}
		printf("|\n");
	}
	printf("%s",sep_row);

	free(sep_row);
}

/*
 * Generates a game board with fixed cells according to user input.
 *
 * Gets a pointer to an already created blank Board.
 * generates a randomized full legal board and stores it in solution.
 */
void generate_user_board(Board* board){
	int fixedCells;
	int row, col;

	printf("Please enter the number of cells to fill [0-80]:\n");
	fixedCells = get_fixed_cells(board);

	backtracking_solution(board, 1);

	while( fixedCells > 0 ){
		col = rand() % (board->board_size);
		row = rand() % (board->board_size);
		if(board->current_board[row][col].isFixed == 0){
			board->current_board[row][col].value = board->solution[row][col].value;
			board->current_board[row][col].isFixed = 1;
			fixedCells--;
			board->num_empty_cells_current--;
		}
	}
}

/*
 * Creates and returns a duplicate of a given game_board. (the actual matrix of cells, not Board).
 */
Cell** copy_game_board(Cell** game_board, int board_size){
	int i, j;
	Cell** copy;
	if((copy = (Cell**) malloc(board_size*sizeof(Cell*))) == NULL){
		fprintf(stderr,"%s",MALLOC_ERROR);
		return NULL;
	}
	for(i = 0; i < board_size; i++){
		copy[i] = (Cell*) malloc(board_size*sizeof(Cell));
		for(j = 0; j < board_size; j++){
			copy[i][j].value = game_board[i][j].value;
			copy[i][j].isFixed = game_board[i][j].isFixed;
		}
	}
	return copy;
}

/*
 * For use in user comand "validate".
 * Checks that the current_board of b is solvable.
 * if it is - sets b->solution to be the found solution.
 * if not - prints a message accordingly.
 */
void validate(Board* b){
	Cell** current_copy = copy_game_board(b->current_board, b->board_size);
	int empty_cells_copy = b->num_empty_cells_current;

	int solvable = backtracking_solution(b, 0);
	if( solvable == 0){
		printf("Validation failed: board is unsolvable\n");
		destroy_game_board(b->current_board,b->board_size);
		b->current_board = current_copy;
		b->num_empty_cells_current = empty_cells_copy;
	}
	else{
		printf("Validation passed: board is solvable\n");
		destroy_game_board(b->solution, b->board_size);
		b->solution = b->current_board;
		b->current_board = current_copy;
		b->num_empty_cells_current = empty_cells_copy;
	}
}

/*
 * For use when user enters command set.
 * If legal and possible, enters inserted_value in to cell in given column and row.
 * If command was legal, prints the new board.
 * If also the board is now full - prints that the user has solved the puzzle.
 */
void set(Board* board, int col, int row, int inserted_val, int param_counter){
	if(col < 0 || row < 0 || board->num_empty_cells_current == 0 || param_counter < 3){
		printf("Error: Invalid Command\n");
		return;
	}
	if(board->current_board[row][col].isFixed == 1) {
		printf("Error: Cell is fixed\n");
		return;
	}
	if(inserted_val == 0 || check_valid_value(board, inserted_val, row, col, 0) == 1) {
		if(inserted_val == 0 && board->current_board[row][col].value != 0) {
			board->num_empty_cells_current++;
		} else {
			if(inserted_val != 0 && board->current_board[row][col].value == 0){
				board->num_empty_cells_current--;
			}
		}
		board->current_board[row][col].value = inserted_val;
		printBoard(board, 0);
		if(board->num_empty_cells_current == 0) {
			printf("Puzzle solved successfully\n");
		}

	}
	else {
		printf("Error: Value is invalid\n");
		return;
	}
	return;
}

/*
 * For use when user enters command restart.
 * restarts the game board from scratch, generating a new puzzle.
 */
void restart(Board* board){
	int i, j;
	board->num_empty_cells_current = board->board_size*board->board_size;
	board->num_empty_cells_solution = board->board_size*board->board_size;
	for(i = 0; i < board->board_size; i++){
		for(j = 0; j < board->board_size; j++){
			createCell(&board->solution[i][j],0);
			createCell(&board->current_board[i][j],0);
		}
	}

	generate_user_board(board);
	printBoard(board,0);
}

void exit_game(Board* board){
	destroyBoard(board);
	printf("Exiting...\n");
	exit(EXIT_SUCCESS);
}

/*
 * The function is called on when user enters the "solve" command.
 * Function is availabe for all modes.
 * Changes game mode to SOLVE_MODE if not already set to it.
 * Tries to load the board saved in path if path is legal and if the file has a legal board on it.
 *
 */
void solve(char** path){

}



/*
 *Recieves given command from user, and implements it appropriately.
 */
void execute_command(Command* command, Board* board) {
	int row = command->params[1] - 1;
	int col = command->params[0] - 1;
	int inserted_val = command->params[2];
	//char* path = command->path_param;

	switch(command->id) {
		case SOLVE:
			break;
		case SET:
			set(board, col, row, inserted_val, command->param_counter);
			break;
		case VALIDATE:
			validate(board);
			break;
		case HINT:
		    if(col < 0 || row < 0 || board->num_empty_cells_current == 0 || command->param_counter < 2){
		    	printf("Error: invalid command\n");
		    	break;
		    }
		    printf("Hint: set cell to %d\n", board->solution[row][col].value);
		    break;
		case RESET:
			restart(board);
			break;
		case EXIT:
			destroy_command_object(command);
			exit_game(board);
			break;
		default :
		    printf("Error: Invalid Command\n");
		    break;
	}
	//free(command);   - gets freed in main now after used
}


