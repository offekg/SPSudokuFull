#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <time.h>

#include "game.h"
#include "parser.h"
#include "solver.h"
#include "SPBufferset.h"
#include "main_aux.h"

#define MAX_COMMAND_SIZE 256

enum game_mode current_mode = INIT_MODE;
int mark_errors = 1;

void eventual_main(){
	Command* command;
	//Board* board;
	char userInput[MAX_COMMAND_SIZE+2] = { 0 };

	opening_message();
	/*board = create_blank_board(2,4);
	generate_user_board(board);
	printBoard(board,0);*/

	while(1){
		printf("\nPlease enter a command:\n");
		if (fgets(userInput, MAX_COMMAND_SIZE+3, stdin) == NULL) {
			if (ferror(stdin)) {
				printf("Error: fgets has failed\n");
			}
			//destroyBoard(board);
			printf("Exiting...\n");
			exit(0);
		}
		if (userInput[MAX_COMMAND_SIZE+1] != 0){
			printf("Error: Invalid Command - Entered more then 256 characters!\n");
			clear_input_line();
			userInput[MAX_COMMAND_SIZE+1] = 0;
			continue;
		}

		command = parse_command(userInput);
		if (!command) {
			/*
			 * Could not parse a legal command, skipping.
			 */
			continue;
		}
		execute_command(command);
		destroy_command_object(command);
	}
	//destroyBoard(board);
}


void check_is_string_int(){
	char* token1 = "32 2";
	char* token2 = "6.2";
	int is1 = is_string_a_int(&token1,strlen(token1));
	int is2 = is_string_a_int(&token2,strlen(token2));
	printf("is1 check: %d\n",is1);
	printf("is2 check: %d\n",is2);
	printf("the result of atoi on long is: %d\n",atoi("0"));
}

void check_fscanf(char* path){
	FILE* file;
	Board* b;
	int m,n,value,i,j;
	int result;
	char is_dot = ' ';
	char* checker[20];// = {0};
	if( (file = fopen(path,"r")) == NULL ){
		printf("Error: failed to open board file at the path you have given -\n%s\n",path);
		//perror("Error: failed to open board file at the path you have given - %s\n%s\n",*path,strerror(errno));
		return;
	}
	if(fscanf(file,"%d",&m) <= 0 || fscanf(file,"%d",&n) <= 0){
		printf("Error: File is not a legal representation of a sudoku board.\n");
		printf("It does not have a legal begining of block size.\n");
		fclose(file);
		return;
		}
	if(m*n > 99){
		printf("Error: The file's given block size is too big.\n");
		fclose(file);
		return;
	}
	printf("n: %d m: %d\n",n,m);
	b = create_blank_board(n,m);
	for(i = 0; i < n*m; i++){
		for(j = 0; j < n*m; j++){
			if( (result = fscanf(file,"%d",&value)) <= 0){
				printf("Error: File is not a legal representation of a sudoku board.\n");
				if(result == 0)
					printf("There are non numrical cells in the file.\n");
				else
					printf("There are not enough cells compared to the block size.\n");
				destroyBoard(b);
				fclose(file);
				return;
				}
			if(value < 0 || value > m*n){
				printf("Error: File is not a legal representation of a sudoku board.\n");
				printf("There is a cell with value %d, that is out of the allowed range 0-%d.\n",value,m*n);
				destroyBoard(b);
				fclose(file);
				return;
			}
			//printf("%d ",value);

			//printCell(b->current_board[i][j]);
			if( (fscanf(file,"%c",&is_dot) != 0) && (is_dot == '.') ){
				if(value == 0){
					printf("Error: File has illegal fixed cell with value 0.\n");
					destroyBoard(b);
					fclose(file);
					return;
				}
				if( check_valid_value(b,value,i,j,0,1) == 0 ){
					printf("Error: File is not a legal representation of a sudoku board.\n");
					printf("There are at least 2 fixed cells that clash with each other.\n");
					destroyBoard(b);
					fclose(file);
					return;
				}
				b->current_board[i][j].isFixed = 1;
			}
			is_dot = ' ';
			(b->current_board[i][j]).value = value;
		}
		//printf("\n");
	}
	if(((m = fscanf(file,"%20s",*checker)) > 0)){
		//printf("fscanf result: %d  checker: %s\n",m,*checker);
		printf("Error: File is not a legal representation of a sudoku board.\n");
		printf("It has too many values compared to the given board size.\n");
		destroyBoard(b);
		fclose(file);
		return;
	}
	fclose(file);
	current_mode = SOLVE;
	printBoard(b,0);
	return;
}


int main(int argc, char *argv[]){
	if(argc > 1)
		srand(atoi(argv[1]));
	SP_BUFF_SET();
	//char *path = "C:\\Users\\offek\\eclipse-c-workspace\\SPSudokuFull\\Board_files\\fixed.txt";
	//check_fscanf(path);
	/*int x;
	char y[5], a[5], b[5], c[5];
	x = scanf("%10s %s %s",y,a,b);
	printf("scanf result: %d\n",x);
	x = scanf("%s\n",c);
	printf("scanf 2 result: %d\n",x);
	printf("y input: %s\na input: %s\nb input: %s\nc input: %s\n",y,a,b,c);*/
	eventual_main();
	return 0;
}


