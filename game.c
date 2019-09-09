#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

#include "game.h"
#include "parser.h"
#include "solver.h"

#define MALLOC_ERROR "Error: malloc has failed"

Board* board = NULL;
/*
 * Prints to the prompt the opening greeting and initial instructions to the user.
 */
void opening_message(){
	printf("WELCOME TO THE GAME SUDOKU!\n\n\n");
	printf("Plesae choose if you would like to edit a game board, solve an existing one or exit.\n");
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
	cell->isError = 0;
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
 * Prints a single cell, acording to the sudoku board format and the status of the cell.
 */
void printCell(Cell* c){

	if( (c->isFixed == 0) && (c->isError == 0 || (current_mode == SOLVE_MODE && mark_errors == 0)) && (c->value != 0) )
		printf(" %2d ",c->value);
	else
		if(c->value != 0 && c->isFixed == 1)
			printf(" %2d.",c->value);
		else
			if(c->value != 0 && c->isError == 1 && (current_mode == EDIT_MODE || mark_errors == 1))
				printf(" %2d*",c->value);
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

	//if(inserted_val == 0 || check_valid_value(board, inserted_val, row, col, 0,0) == 1) {
	if(inserted_val == 0 && game_board[row][col].value != 0)
		board->num_empty_cells_current++;
	if(inserted_val != 0 && game_board[row][col].value == 0)
		board->num_empty_cells_current--;

	game_board[row][col].value = inserted_val;
	mark_erroneous_cells(b,row,col);
	printBoard(board, 0);
	printf("num empty cells: %d\n",board->num_empty_cells_current);
	if(board->num_empty_cells_current == 0 && current_mode == SOLVE_MODE) {
		if(check_board_errors(b) == 0){
			printf("Congratulations! Puzzle solved successfully!!\n");
			destroyBoard(b);
			current_mode = INIT_MODE;
		}
		else{
			printf("Sorry, your current solution has errors :(  Keep trying!\n");
			printf("You can undo yout last move or set cells to a diffrent value.\n");
		}
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
 * Function that recieves a path, and if possible loads the game board that is saved on it.
 * Returns 1 on success. 0 if failed to load.
 *
 */
int load_board(char* path){
	FILE* file;
	int m,n,value,i,j;
	int result;
	int num_empty_cells = 0;
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
			if( (fscanf(file,"%c",&is_dot) != 0) && (is_dot == '.') ){
				if(value == 0){
					printf("Error: File is not a legal representation of a sudoku board.\n");
					printf("EFile has an illegal fixed cell with value 0.\n");
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
			is_dot = ' ';

			(board->current_board[i][j]).value = value;
			if(value == 0)
				num_empty_cells++;
			else
				mark_erroneous_cells(board,i,j);
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
	board->num_empty_cells_current = num_empty_cells;
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
	if(load_board(path) == 0)
		return;

	if(current_mode != SOLVE_MODE){
		current_mode = SOLVE_MODE;
		printf("Game is now in SOLVE mode\n");
	}
	printBoard(board,0);
}


void edit(char* path){
	if(path != NULL){
		if(load_board(path) == 0)
			return;
	}
	else{

	}
	if(current_mode != EDIT_MODE)
		current_mode = EDIT_MODE;
	printBoard(board,0);
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
			validate(board);
			break;
		case HINT:
		    if(col < 0 || row < 0 || board->num_empty_cells_current == 0 || command->param_counter < 2){
		    	printf("Error: Invalid command\n");
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
		default:
		    printf("Error: Invalid Command\n");
		    break;
	}
	return;
	//free(command);   - gets freed in main now after used
}


