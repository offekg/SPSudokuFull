#include <string.h>

#include "main_aux.h"

int get_fixed_cells() {
	int fixedCells;
	if (scanf("%d", &fixedCells) != 1) {
		checkEOF();
	}
	while (fixedCells < 0 || fixedCells > 80){
		printf("Error: invalid number of cells to fill (should be between 0 and 80)\n");
		if (scanf("%d", &fixedCells) != 1) {
			checkEOF();
		}
	}
	return fixedCells;
}
