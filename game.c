#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <ctype.h>

#include "game.h"
#include "parser.h"
#include "solver.h"
#include "stack.h"
#include "linked_list.h"
#include "gurobi_utils.h"


Board* board = NULL;


/*
 * The following 3 functions are printinf functions for use after changing
 * the game mode.
 * They inform the player of the available commands.
 */
void INIT_Mode_print(){
	printf("Game is now in INIT mode.\n");
	printf("In this mode you may use the following commands:\n");
	printf("    solve, edit or exit\n");
}

void SOLVE_Mode_print(){
	printf("Game is now in SOLVE mode.\n");
	printf("In this mode you may use the following commands:\n");
	printf("    solve, edit, print_board, mark_errors, set, validate, undo,\n");
	printf("    redo, save, hint, autofill, num_solutions, reset or exit\n");
}

void EDIT_Mode_print(){
	printf("Game is now in EDIT mode.\n");
	printf("In this mode you may use the following commands:\n");
	printf("    solve, edit, print_board, set, validate, undo, redo,\n");
	printf("    save, num_solutions, generate, reset or exit\n");
}

/*
 * Prints to the prompt the opening greeting and initial instructions to the user.
 */
void opening_message(){
	printf("WELCOME TO THE GAME SUDOKU!\n\n\n");
	printf("Plesae choose if you would like to edit a game board, solve an existing one or exit.\n");
	printf("Have fun!\n\n");
	INIT_Mode_print();
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
 * Function checks if the given board has no more empty cells.
 * If board is full - return 1; otherwise return 0;
 * If board is full the function checks if the board has errors:
 *     If there aren't, then the game mode is switched to INIT,
 *     and the game board is destroyed.
 *     A message is printed acourdingly.
 * to_print: if it is 1, messeges are printed. otherwise it isn't.
 */
int check_full_board(Board* b, int to_print){
	if(b->num_empty_cells_current == 0 && current_mode == SOLVE_MODE) {
		if(check_board_errors(b) == 0){
			destroyBoard(b);
			if(to_print == 1){
				printf("Congratulations! Puzzle solved successfully!!\n\n");
				board = NULL;
			}
			current_mode = INIT_MODE;
			INIT_Mode_print();
		}
		else{
			if(to_print == 1){
				printf("Sorry, your current solution has errors :(  Keep trying!\n");
				printf("You can undo your last move or set cells to a diffrent value.\n");
			}
		}
		return 1;
	}
	return 0;
}


/*
 * Function receives a board, a cell's row&col and a value;
 * The function inserts the value in the cell, without any checks.
 * Also marks or unmarks errors appropriately.
 * num_empty_cells is updated appropriately.
 */
void set_value_simple(Board* b, int row, int col, int inserted_val){
	if(inserted_val == 0 && b->current_board[row][col].value != 0)
		b->num_empty_cells_current++;
	if(inserted_val != 0 && b->current_board[row][col].value == 0)
		b->num_empty_cells_current--;

	b->current_board[row][col].value = inserted_val;
	mark_erroneous_cells(b, row, col);
}

/*
 * For use when user enters command set.
 * If legal and possible, enters inserted_value in to cell in given column and row.
 * If command was legal, prints the new board.
 * If also the board is now full - prints that the user has solved the puzzle.
 */
void set(Board* b, int col, int row, int inserted_val) {
	Cell** game_board = b->current_board;
	MovesList* moves = initialize_move_list();
	int board_size = b->board_size;
	TurnsList* turns = b->turns;

	if(col < 0 || col > board_size){
		printf("Error: Invalid Command - Column (first) paramater is out of the range 1-%d.\n",board_size);
		return;
	}
	if(row < 0 || row > board_size){
			printf	("Error: Invalid Command - Row (second) paramater is out of the range 1-%d.\n",board_size);
			return;
	}
	if(inserted_val < 0 || inserted_val > board_size){
			printf("Error: Invalid Command - The inserted value (third) paramater is out of the range 1-%d.\n",board_size);
			return;
	}
	if(current_mode == SOLVE_MODE && game_board[row][col].isFixed == 1) {
		printf("Error: You can't change a fixed cell in Solve Mode.\n");
		return;
	}

	add_move(moves, row, col, game_board[row][col].value, inserted_val);
	add_turn(turns, moves);

	set_value_simple(b, row, col, inserted_val);


	printBoard(board, 0);
	/*printf("num empty cells: %d\n",board->num_empty_cells_current);*/
	check_full_board(b,1);
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

void exit_game(Board* board){ /*^^^check need to destroy turns list^^^*/
	destroyBoard(board);
	printf("Now Exiting The Game\nGoodbye!");
	exit(EXIT_SUCCESS);
}

/*
 * Function that recieves a path, and if possible loads the game board that is saved on it.
 * Returns 1 on success. 0 if failed to load.
 * if mode == 0, for solve mode;
 * mode == 1 for edit mode, and doesn't check for erroneous fixed cells.
 *
 */
int load_board(char* path, enum game_mode mode){
	FILE* file;
	int m,n,value,i,j;
	int result;
	char is_dot = ' ';
	char* checker[20];
	if( (file = fopen(path,"r")) == NULL ){
		printf("Error: failed to open board file at the path you have given -\n%s\n",path);
		return 0;
	}
	if(fscanf(file,"%d",&m) <= 0 || fscanf(file,"%d",&n) <= 0){
		printf("Error: File is not a legal representation of a sudoku board.\n");
		printf("It does not have a legal begining of block size.\n");
		fclose(file);
		return 0;
		}
	if(m*n > 99){
		printf("Error: The file's given block size is too big.\n");
		fclose(file);
		return 0;
	}
	board = create_blank_board(n,m);
	for(i = 0; i < n*m; i++){
		for(j = 0; j < n*m; j++){
			if( (result = fscanf(file,"%d",&value)) <= 0){
				printf("Error: File is not a legal representation of a sudoku board.\n");
				if(result == 0)
					printf("There are non numrical cells in the file.\n");
				else
					printf("There are not enough cells compared to the block size.\n");
				destroyBoard(board);
				board = NULL;
				fclose(file);
				return 0;
				}
			if(value < 0 || value > m*n){
				printf("Error: File is not a legal representation of a sudoku board.\n");
				printf("There is a cell with value %d, that is out of the allowed range 0-%d.\n",value,m*n);
				destroyBoard(board);
				board = NULL;
				fclose(file);
				return 0;
			}
			/*printf("%d ",value);*/

			/*printCell(b->current_board[i][j]);*/
			if(  (fscanf(file,"%c",&is_dot) != 0) && (mode == SOLVE_MODE) && (is_dot == '.') ){
				/*printf("marking %d,%d as fixed\n",j+1,i+1);*/
				if(value == 0){
					printf("Error: File is not a legal representation of a sudoku board.\n");
					printf("File has an illegal fixed cell with value 0.\n");
					destroyBoard(board);
					board = NULL;
					fclose(file);
					return 0;
				}
				if( check_valid_value(board,value,i,j,0,1) == 0 ){
					printf("Error: File is not a legal representation of a sudoku board.\n");
					printf("There are at least 2 fixed cells that clash with each other.\n");
					destroyBoard(board);
					board = NULL;
					fclose(file);
					return 0;
				}
				board->current_board[i][j].isFixed = 1;
			}
			/*printf("cell %d,%d is_dot: %c\n",j+1,i+1,is_dot);*/
			if( is_dot != '.' &&  !isspace(is_dot) ){
				printf("Error: File is not a legal representation of a sudoku board.\n");
				printf("There are non numrical cells in the file: %c.\n",is_dot);
				destroyBoard(board);
				board = NULL;
				fclose(file);
				return 0;
			}
			is_dot = ' ';
			set_value_simple(board, i, j, value);
			//mark_erroneous_cells(board,i,j);

			/*/printf("cell %d,%d isError: %d\n",j+1,i+1,board->current_board[i][j].isError);*/
		}
	}
	if(((m = fscanf(file,"%20s",*checker)) > 0)){
		/*printf("fscanf result: %d  checker: %s\n",m,*checker);*/
		printf("Error: File is not a legal representation of a sudoku board.\n");
		printf("It has too many values compared to the given board size.\n");
		destroyBoard(board);
		fclose(file);
		return 0;
	}

	fclose(file);
	return 1;
}

/*
 * The function is called on when user enters the "solve" command.
 * Function is availabe for all modes.
 * Changes game mode to SOLVE_MODE if not already set to it.
 * Tries to load the board saved in path if path is legal and if the file has a legal board on it.
 *
 */
void solve(char* path){
	if(board){
		destroyBoard(board);
		board = NULL;
	}
	if(load_board(path,SOLVE_MODE) == 0)
		return;

	if(current_mode != SOLVE_MODE){
		current_mode = SOLVE_MODE;
		SOLVE_Mode_print();
	}

	printBoard(board,0);
	check_full_board(board,1);
}


void edit(char* path){
	if(board){
		destroyBoard(board);
		board = NULL;
	}
	if(path != NULL){
		if( load_board(path,EDIT_MODE) == 0 )
			return;
	}
	else
		board = create_blank_board(3,3);
	if(current_mode != EDIT_MODE){
		current_mode = EDIT_MODE;
		EDIT_Mode_print();
	}
	printBoard(board,0);
}

int autofill(Board** board){
	int i,j;
	int num_filled = 0;
	int* options;
	MovesList* moves;
	int board_size = (*board)->board_size;
	Board* updated_board = copy_Board(*board);
	TurnsList* turns = updated_board->turns;

	if(board == NULL){
		printf("Error: There is no board to autofill.\n");
		return -1;
	}
	moves = initialize_move_list();

	/*printf("num empty cells now is: %d\n",updated_board->num_empty_cells_current);*/
	for(i = 0; i < board_size; i++)
		for(j = 0; j < board_size; j++){
			if((*board)->current_board[i][j].value == 0){
				options = generate_options(*board,i,j,0);
				/*printf("num options for cell %d,%d is %d\n",i,j,options[0]);*/
				if(options[0] == 1){
					num_filled++;
					add_move(moves, i, j, updated_board->current_board[i][j].value, options[1]);
					set_value_simple(updated_board,i,j,options[1]);
					/*mark_erroneous_cells(updated_board, i, j);*/
				}
				free(options);
			}
		}
	printf("num empty cells after filling in copy is: %d\n",updated_board->num_empty_cells_current);
	add_turn(turns, moves);
	destroyBoard(*board);
	*board = updated_board;

	printf("Successfully filled %d cells\n", num_filled);
	printBoard(*board,0);
	/*printf("num empty cells now is: %d\n",board->num_empty_cells_current);*/
	check_full_board(*board,1);
	return num_filled;
}

/*
 * Function uses ILP to check if the given board has a solution
 * or not.
 * Returns 1 if a solutions was found.
 * Otherwise returns 0.
 */
int validate_board(Board* board){
	Board* b_copy = copy_Board(board);

	if(check_board_errors(b_copy) == 1){
		printf("Error: The board has erroneous cells.\n");
		return 0;
	}

	if(find_ILP_solution(b_copy,0) == 1){
		destroyBoard(b_copy);
		return 1;
	}
	destroyBoard(b_copy);
	return 0;
}

/*
 * Function for use of hint command.
 * Receives the board, and cell col and row.
 * Checks everything is legal, and looks for a ilp solution to the board.
 * If there is a solution, function prints the value of solution in the cell.
 */
void cell_hint(Board* b, int col, int row){
	Board* b_copy;
	int value;
	int board_size = b->board_size;

	if(col < 0 || col > board_size){
		printf("Error: Invalid Command - Column (first) paramater is out of the range 1-%d.\n",board_size);
		return;
	}
	if(row < 0 || row > board_size){
		printf	("Error: Invalid Command - Row (second) paramater is out of the range 1-%d.\n",board_size);
		return;
	}
	if(check_board_errors(b) == 1){
		printf("Error: The board has erroneous cells.\n");
		return;
	}
	if(b->current_board[row][col].isFixed == 1){
		printf("Error: The cell you asked a hint fot is fixed.\n");
		return;
	}
	if(b->current_board[row][col].value > 0){
		printf("Error: The cell you asked a hint for already has a value.\n");
		return;
	}

	b_copy = copy_Board(b);
	if(find_ILP_solution(b_copy,1) == 0){
		destroyBoard(b_copy);
		printf("Error: The board has no solution.\n");
		return;
	}

	value = b_copy->current_board[row][col].value;
	printf("Hint: You can set cell <%d,%d> to the value %d.\n",col+1,row+1,value);
	destroyBoard(b_copy);
	return;
}

/*
 * Function receives a path (full or relative), and tries to save the current board
 * in the file at the path given, by known format.
 * In EDIT mode:
 * 		- Erroneous boards or boards with no solution won't be saved.
 * 		- All cells are marked as fixed.
 */
void save(char* path){
	FILE* file;
	int i,j;
	Cell* cell;

	if(current_mode == EDIT_MODE){
		if(check_board_errors(board) == 1){
			printf("Error: The board currently has errors, so it can't be saved.\n");
			return;
		}
		if(validate_board(board) == 0){
			printf("Error: The board has no solution, so it can't be saved.\n");
			return;
		}
	}

	if( (file = fopen(path,"w")) == NULL ){
		printf("Error: failed to open board file at the path you have given -\n%s\n",path);
		return;
	}

	fprintf(file,"%d %d\n", board->block_rows, board->block_cols);
	for(i = 0; i < board->board_size; i++){
		for(j = 0; j < board->board_size; j++){
			cell = &(board->current_board[i][j]);
			fprintf(file,"%d", cell->value);
			if((current_mode == EDIT_MODE && cell->value != 0) || cell->isFixed == 1)
				fprintf(file,".");
			if(j != board->board_size - 1)
				fprintf(file," ");
		}
		fprintf(file,"\n");
	}
	fclose(file);
}


/* Function for use when user enters command undo.
 * If legal and possible, reverts the last move the user made
 * If command was legal, prints the changes and the board.
 */
void undo(Board* b, int to_print){
	Node* move;
	TurnsList* turns = b->turns;

	if (turns->position_in_list == 0) {
		printf("No turns to undo\n");
		return;
	}

	move = turns->current_move->current_changes->top;
	while (move) {
		set_value_simple(b, move->row, move->col, move->previous_val);
		if (to_print) {
			printf("Cell <%d,%d> has been modified back to %d\n", move->col + 1,
					move->row + 1, move->previous_val);
		}
		move = move->next;
	}

	if (turns->position_in_list != 1) {
		turns->current_move = turns->current_move->previous;
	}
	else{
		turns->current_move = NULL;
	}

	turns->position_in_list -= 1;
	if(to_print)
		printBoard(b,0);
	return;
}

/*
 * For use when user enters command redo.
 * If legal and possible, redos the last move the user made
 * If command was legal, prints the changes and the board.
 */
void redo(Board* b, int to_print){
	Node* move;
	TurnsList* turns = b->turns;

	if (turns->length == 0 || turns->position_in_list == turns->length) {
		printf("There are no turns to redo\n");
		return;
	}

	if (turns->position_in_list == 0){
		move = turns->top->current_changes->top;
	} else{
		move = turns->current_move->next->current_changes->top;
	}

	while (move) {
		set_value_simple(b, move->row, move->col, move->new_val);
		if (to_print) {
			printf("Cell <%d,%d> has been modified back to %d\n", move->col + 1,
					move->row + 1, move->new_val);
		}
		move = move->next;
	}

	if (turns->position_in_list != 0) {
		turns->current_move = turns->current_move->next;
	} else{
		turns->current_move = turns->top;
	}
	turns->position_in_list += 1;
	printBoard(b,0);
	return;
}

/*
 * For use when user enters command reset.
 */
void reset_board(Board* b) {
	/*printf("Now reseting the board back to original configuration...\n");*/
	while (b->turns->position_in_list > 0) {
		undo(b, 0);
	}
	printBoard(b,0);
}



/*
 *Recieves given command from user, and implements it appropriately.
 */
void execute_command(Command* command){
	int col = command->params[0] - 1;
	int row = command->params[1] - 1;
	int inserted_val = command->params[2];
	int binary_param = command->params[0];

	switch(command->id) {
		case SOLVE:
			solve(command->path_param);
			break;
		case EDIT:
			edit(command->path_param);
			break;
		case MARK_ERRORS:
			if(binary_param > 1 || binary_param < 0)
				printf("Error: Invalid Command - mark_errors can only be used with 0 or 1.\n");
			else{
				mark_errors = binary_param;
				printBoard(board,0);
			}
			break;
		case PRINT_BOARD:
			printBoard(board,0);
			break;
		case SET:
			set(board, col, row, inserted_val);
			break;
		case VALIDATE:
			if(validate_board(board) == 1)
				printf("Board validated successfully. A solutions exists.\n");
			else
				printf("The board has no solution.\n");
			break;
		case GENERATE:
			break;
		case UNDO:
			undo(board, 1);
			break;
		case REDO:
			redo(board, 1);
			break;
		case SAVE:
			save(command->path_param);
			break;
		case HINT:
		    cell_hint(board, col, row);
		    break;
		case NUM_SOLUTIONS:
			printf("Now starting to calculate number of solutions.\nThis could take a while.\n\n");
			printf("The number of solutions for the current board is %d\n",num_solutions(board));
			break;
		case AUTOFILL:
			autofill(&board);
			break;
		case RESET:
			reset_board(board);
			break;
		case EXIT:
			destroy_command_object(command);
			exit_game(board);
			break;
		default:
		    printf("Error: Invalid Command\n");
		    break;
	}
	/*print_turns(board->turns);*/
	/*printf("num empty cells: %d\n",board->num_empty_cells_current);*/
	return;
}


