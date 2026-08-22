#include <stdbool.h>
#include "stack-structs.h"

Stack *newStack(size_t capacity, size_t growthStep);
Stack *defaultStack(void);
Stack *pushStack(Stack *stack, void *item);
StackPop popStack(Stack *stack, bool resize);
void *peekStack(Stack *stack);
void freeStack(Stack *stack, void (*f)(void *));
void freeStackAndItems(Stack *stack);