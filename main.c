#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include "game.h"
#include "parser.h"
#include "solver.h"
#include "SPBufferset.h"

void board_test(){
	Cell* c = (Cell*) malloc(sizeof(Cell));
	createCell(c,5);
	printf("Cell's value: %d, Cell's fixed: %d.\n",c->value,c->isFixed);
	c->value += 3;
	c->isFixed = 1;
	printf("after change: Cell's value: %d, Cell's fixed: %d.\n",c->value,c->isFixed);
	Board* b = createBoard(9,3,3);
	for(int i = 0; i < 9; i++){
		for(int j = 0; j < 9; j++){
			printf("%d ",(b->current_board[i][j]).value);
			(b->current_board[i][j]).value = i+j;
		}
		printf("\n");
	}
	printf("\n\n");
	for(int i = 0; i < 9; i++){
		for(int j = 0; j < 9; j++){
			printf("%d ",(b->current_board[i][j]).value);
		}
		printf("\n");
	}
	destroyBoard(b);
}

int main(){
	SP_BUFF_SET();
	printf("Starting");
	board_test();

	return 0;
}
