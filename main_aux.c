#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void checkEOF(){
	if (feof(stdin)) {
		printf("Exiting...\n");
		exit(EXIT_FAILURE);
	}
}
