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
#include "board_utils.h"

#define MAX_COMMAND_SIZE 256

enum game_mode current_mode = INIT_MODE;
int mark_errors = 1;

void eventual_main(){
	Command* command;
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



int main(int argc, char *argv[]){
	if(argc > 1)
		srand(atoi(argv[1]));
	SP_BUFF_SET();
	/*char *path = "C:\\Users\\offek\\eclipse-c-workspace\\SPSudokuFull\\Board_files\\fixed.txt";
	check_fscanf(path);
	int x;
	char y[5], a[5], b[5], c[5];
	x = scanf("%10s %s %s",y,a,b);
	printf("scanf result: %d\n",x);
	x = scanf("%s\n",c);
	printf("scanf 2 result: %d\n",x);
	printf("y input: %s\na input: %s\nb input: %s\nc input: %s\n",y,a,b,c);*/
	eventual_main();
	return 0;
}


