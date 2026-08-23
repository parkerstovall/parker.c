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

#endif // STACK_STRUCTS_C