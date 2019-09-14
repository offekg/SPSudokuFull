#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <ctype.h>

#include "game.h"
#include "parser.h"
#include "solver.h"
#include "stack.h"

//#define MALLOC_ERROR "Error: malloc has failed"

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
 */
int check_full_board(Board* b){
	if(b->num_empty_cells_current == 0 && current_mode == SOLVE_MODE) {
		if(check_board_errors(b) == 0){
			printf("Congratulations! Puzzle solved successfully!!\n\n");
			destroyBoard(b);
			current_mode = INIT_MODE;
			INIT_Mode_print();
		}
		else{
			printf("Sorry, your current solution has errors :(  Keep trying!\n");
			printf("You can undo your last move or set cells to a diffrent value.\n");
		}
		return 1;
	}
	return 0;
}


/*
 * Function receives a board, a cell's row&col and a value;
 * The function inserts the value in the cell, without any checks.
 * num_empty_cells is updated appropriately.
 */
void set_value_simple(Board* b, int row, int col, int inserted_val){
	if(inserted_val == 0 && b->current_board[row][col].value != 0)
		board->num_empty_cells_current++;
	if(inserted_val != 0 && b->current_board[row][col].value == 0)
		board->num_empty_cells_current--;

	b->current_board[row][col].value = inserted_val;
}

/*
 * For use when user enters command set.
 * If legal and possible, enters inserted_value in to cell in given column and row.
 * If command was legal, prints the new board.
 * If also the board is now full - prints that the user has solved the puzzle.
 */
void set(Board* b, int col, int row, int inserted_val){
	Cell** game_board = b->current_board;
	int board_size = b->board_size;
	if(col < 0 || col > board_size){
		printf("Error: Invalid Command - Column (first) paramater is out of the range 1-%d.\n",board_size);
		return;
	}
	if(row < 0 || row > board_size){
			printf("Error: Invalid Command - Row (second) paramater is out of the range 1-%d.\n",board_size);
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

	set_value_simple(b, row, col, inserted_val);

	mark_erroneous_cells(b, row, col);
	printBoard(board, 0);
	//printf("num empty cells: %d\n",board->num_empty_cells_current);
	check_full_board(b);
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
	//int num_empty_cells = 0;
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
	//printf("n: %d m: %d\n",n,m);
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
			//printf("%d ",value);

			//printCell(b->current_board[i][j]);
			if(  (fscanf(file,"%c",&is_dot) != 0) && (mode == SOLVE_MODE) && (is_dot == '.') ){
				//printf("marking %d,%d as fixed\n",j+1,i+1);
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
			//printf("cell %d,%d is_dot: %c\n",j+1,i+1,is_dot);
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
			mark_erroneous_cells(board,i,j);
			//printf("cell %d,%d isError: %d\n",j+1,i+1,board->current_board[i][j].isError);
		}
	}
	if(((m = fscanf(file,"%20s",*checker)) > 0)){
		//printf("fscanf result: %d  checker: %s\n",m,*checker);
		printf("Error: File is not a legal representation of a sudoku board.\n");
		printf("It has too many values compared to the given board size.\n");
		destroyBoard(board);
		fclose(file);
		return 0;
	}
	//board->num_empty_cells_current = num_empty_cells;
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
	check_full_board(board);
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

int autofill(){
	int i,j;
	int num_filled = 0;
	int* options;
	int board_size = board->board_size;
	Board* updated_board = copy_Board(board);
	if(board == NULL){
		printf("Error: there is no board to autofill.\n");
		return -1;
	}
	//printf("num empty cells now is: %d\n",board->num_empty_cells_current);
	for(i = 0; i < board_size; i++)
		for(j = 0; j < board_size; j++){
			if(board->current_board[i][j].value == 0){
				options = generate_options(board,i,j,0);
				//printf("num options for cell %d,%d is %d\n",i,j,options[0]);
				if(options[0] == 1){
					set_value_simple(updated_board,i,j,options[1]);
					//updated_game_board[i][j].value = options[1];
					num_filled++;

					mark_erroneous_cells(updated_board, i, j);
				}
				free(options);
			}
		}
	//destroy_game_board(board->current_board,board_size);
	//board->current_board = updated_board;
	destroyBoard(board);
	board = updated_board;
	//board->num_empty_cells_current -= num_filled;
	printf("Successfully filled %d cells\n", num_filled);
	printBoard(board,0);
	//printf("num empty cells now is: %d\n",board->num_empty_cells_current);
	check_full_board(board);
	return num_filled;
}

/*
 * Function uses ILP to check if the given board has a solution
 * or not.
 * Returns 1 if a solutions was found.
 * Otherwise returns 0.
 */
int validate_board(Board* board){

	return 0;
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
	//fprintf(file,"sdfsdfsd");
	fclose(file);
}

/*
 *Recieves given command from user, and implements it appropriately.
 */
void execute_command(Command* command){
	int col = command->params[0] - 1;
	int row = command->params[1] - 1;
	int inserted_val = command->params[2];
	int binary_param = command->params[0];
	//char* path = command->path_param;

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
			//validate(board);
			break;
		case GENERATE:
			break;
		case UNDO:
			break;
		case REDO:
			break;
		case SAVE:
			save(command->path_param);
			break;
		case HINT:
		    if(col < 0 || row < 0 || board->num_empty_cells_current == 0 || command->param_counter < 2){
		    	printf("Error: Invalid command\n");
		    	break;
		    }
		    //printf("Hint: set cell to %d\n", board->solution[row][col].value);
		    break;
		case NUM_SOLUTIONS:
			printf("Now starting to calculate number of solutions.\nThis could take a while.\n\n");
			printf("The number of solutions for the current board is %d\n",num_solutions(board));
			break;
		case AUTOFILL:
			autofill();
			break;
		case RESET:
			restart(board);
			break;
		case EXIT:
			destroy_command_object(command);
			exit_game(board);
			break;
		default:
		    printf("Error: Invalid Command\n");
		    break;
	}
	//printf("num empty cells: %d\n",board->num_empty_cells_current);
	return;
}


