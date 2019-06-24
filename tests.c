#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "game.h"
#include "parser.h"
#include "solver.h"

int main(int argc, char *argv[])
{
	Cell* c = createCell(5);
	printf("Cell's value: %d, Cell's fixed: %d.\n",c->value,c->isFixed);
	c->value += 3;
	c->isFixed = 1;
	printf("after change: Cell's value: %d, Cell's fixed: %d.\n",c->value,c->isFixed);
	return 0;
}
