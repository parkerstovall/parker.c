#include <stdbool.h>
#include "stack-structs.h"

Stack *newStack(size_t capacity, size_t growthStep);
Stack *defaultStack(void);
Stack *pushStack(Stack *stack, void *item);
void *popStack(Stack *stack, bool resize);
void *peekStack(Stack *stack);
void *swapStack(Stack *stack, void *item);
void freeStack(Stack *stack, void (*f)(void *));
void freeStackAndItems(Stack *stack);