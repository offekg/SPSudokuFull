#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "game.h"
#include "board_utils.h"

void checkEOF(Board* board){
	if (feof(stdin)) {
		destroyBoard(board);
		printf("Exiting...\n");
		exit(EXIT_SUCCESS);
	}
}

void clear_input_line(){
	char c;
	do{
		c = fgetc(stdin);
		if (ferror(stdin)) {
			printf("Error: fgets has failed\n");
			printf("Exiting...\n");
			exit(0);
		}
	}
	while(c != '\n' && c != EOF);
	return;
}

