#ifndef STACK_STRUCTS_C /* Include guard */
#define STACK_STRUCTS_C

#include <stdio.h>

typedef struct Stack
{
    size_t size;
    size_t capacity;
    size_t growthStep;
    void **items;
} Stack;

typedef struct StackPop
{
    Stack *stack;
    void *item;
} StackPop;

#endif // STACK_STRUCTS_C