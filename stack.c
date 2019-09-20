/*
 * The "stack" module contains the stack struct and relevant use functions,
 * that are used for simulating recursion for Exhaustive Backtracking (num_solutions)
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

#include "board_utils.h"
#include "stack.h"


/*
 * Function that creates an empty Stack struct.
 * Initializes the amount of elements (count) to 0, and top element to NULL.
 * Returns a pointer to the stack.
 */
Stack* initialize_stack(){
	Stack* stack = (Stack*)malloc(sizeof(Stack));
	if(stack == NULL){
		printf(MALLOC_ERROR);
		exit(0);
	}
	stack->count = 0;
	stack->top = NULL;
	return stack;
}

/*
 * Recieves a stack and safeley frees all related memory (including all elemants).
 */
void destroy_stack(Stack* stk){
	while(!is_empty(stk))
		free(pop(stk));

	free(stk);
}


/*
 * Creates a new elem out of the given data, and puts it at top of the stack.
 * Needs to get the wanted cell's row, column and value.
 */
void push(Stack* stk, int row, int col, int value){
	StackElem* elem = (StackElem*)malloc(sizeof(StackElem));
	if(elem == NULL){
		printf(MALLOC_ERROR);
		exit(0);
	}
	elem->row = row;
	elem->col = col;
	elem->value = value;
	elem->next = stk->top;
	stk->top = elem;
	stk->count++;
}

/*
 * The function removes and returns a pointer to the top elemant
 * of the given stack. If stack is empty, returns NULL.
 * When you finish with the elemant, you have to free it.
 */
StackElem* pop(Stack* stk){
	StackElem* temp;

	if(is_empty(stk))
		return NULL;

	temp = stk->top;
	stk->top = (stk->top)->next;
	stk->count--;
	return temp;
}

/*
 * The function returns a pointer to the top elemant of the given stack.
 * If stack is empty, returns NULL
 */
StackElem* top(Stack* stk){
	return(stk->top);
}

/*
 * Function checks if the given stack is empty.
 * Returns 1 if it is empty, 0 otherwise.
 */
int is_empty(Stack* stk){
	if(stk->count == 0)
		return 1;

	return 0;
}

void print_StackElem(StackElem* elem){
	printf("elem is: %d,%d, value-%d\n",elem->col+1,elem->row+1,elem->value);
}

void print_Stack(Stack* stk){
	StackElem* temp = stk->top;
	int count = 1;
	while(temp != NULL){
		printf("%d",count);
		print_StackElem(temp);
		temp = temp->next;
		count++;
	}
	printf("***end of stack***\n");
}


