#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "game.h"

void checkEOF(Board* board){
	if (feof(stdin)) {
		destroyBoard(board);
		printf("Exiting...\n");
		exit(EXIT_SUCCESS);
	}
}
