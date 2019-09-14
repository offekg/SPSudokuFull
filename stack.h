
#ifndef STACK_H_
#define STACK_H_

#define EMPTY 0


/*
 * Structure: elem
 * 		Used to represent an elemant in the stack of board cell with value.
 *
 * 		row: represents the wanted cell's row.
 * 		col: represents the wanted cell's column
 * 		value: represents the wanted cell's value
 * 		next: represents the next elemant in the stack (below this one)
 *
 */
typedef struct elem{
	int row;
	int col;
	int value;
	struct elem* next;
} StackElem;


/*
 * Structure: Stack
 * 		Used to represent a stack of board cells and values.
 *
 * 		count: The amount of elements currently in the stack.
 * 		top: The first (top) element in the stack.
 */
typedef struct stack{
	int count;
	StackElem* top;
} Stack;


/*
 * Function that creates an empty Stack struct.
 * Initializes the amount of elements (count) to 0, and top element to NULL.
 * Returns a pointer to the stack.
 */
Stack* initialize_stack();

/*
 * Recieves a stack and safley frees all related memory (including all elemants).
 */
void destroy_stack(Stack* stk);


/*
 * Creates a new elem out of the given data, and puts it at top of the stack.
 * Needs to get the wanted cell's row, column and value.
 */
void push(Stack* stk, int row, int col, int value);

/*
 * The function removes and returns a pointer to the top elemant
 * of the given stack.
 */
StackElem* pop(Stack* stk);

/*
 * The function returns a pointer to the top elemant of the given stack.
 */
StackElem* top(Stack* stk);

/*
 * Function checks if the given stack is empty.
 * Returns 1 if it is empty, 0 otherwise.
 */
int is_empty(Stack* stk);

void print_StackElem(StackElem* elem);

void print_Stack(Stack* stk);

#endif /* STACK_H_ */
