#include <errno.h>
#include <stdlib.h>
#include <stdbool.h>
#include "stack-structs.h"

Stack *newStack(int capacity, int growthStep)
{
    if (capacity <= 0 || growthStep <= 0)
    {
        perror("stack -> all arguments should be higher than zero");
        exit(EXIT_FAILURE);
    }

    Stack *stack = malloc(sizeof(Stack));
    if (!stack)
    {
        printf("Error Code: %d\n", errno);
        perror("stack-");
        exit(EXIT_FAILURE);
    }

    stack->size = 0;
    stack->capacity = (size_t)capacity;
    stack->growthStep = (size_t)growthStep;
    stack->items = malloc(capacity * sizeof(void *));
    if (!stack->items)
    {
        printf("Error Code: %d\n", errno);
        perror("stack->items");
        free(stack);
        exit(EXIT_FAILURE);
    }

    return stack;
}

Stack *defaultStack(void)
{
    return newStack(1, 1);
}

Stack *pushStack(Stack *stack, void *item)
{
    if (stack->capacity < stack->size + 1)
    {
        size_t newCapacity = stack->capacity + stack->growthStep;
        void **tempItems = realloc(stack->items, newCapacity * sizeof(void *));
        if (!tempItems)
        {
            printf("Error Code: %d\n", errno);
            perror("stack: tempItems");
            exit(EXIT_FAILURE);
        }

        stack->capacity = newCapacity;
        stack->items = tempItems;
    }

    stack->items[stack->size] = item;
    stack->size++;
    return stack;
}

void *popStack(Stack *stack, bool resize)
{
    if (stack->size <= 0)
    {
        return NULL;
    }

    void *item = stack->items[stack->size - 1];

    stack->items[stack->size - 1] = NULL;
    stack->size--;

    if (
        resize &&
        stack->capacity >= stack->growthStep &&
        stack->capacity - stack->growthStep >= stack->size)
    {
        size_t newCapacity = stack->capacity - stack->growthStep;
        void **tempItems = realloc(stack->items, newCapacity * sizeof(void *));
        if (!tempItems && newCapacity > 0)
        {
            printf("Error Code: %d\n", errno);
            perror("stack: tempItems");
            exit(EXIT_FAILURE);
        }

        stack->capacity = newCapacity;
        stack->items = tempItems;
    }

    return item;
}

void *peekStack(Stack *stack)
{
    if (stack->size <= 0)
    {
        return NULL;
    }

    return stack->items[stack->size - 1];
}

void *swapStack(Stack *stack, void *newItem)
{
    if (stack->size <= 0)
    {
        return NULL;
    }

    void *oldItem = stack->items[stack->size - 1];
    stack->items[stack->size - 1] = newItem;

    return oldItem;
}

void freeStack(Stack *stack, void (*f)(void *))
{
    if (!stack)
    {
        return;
    }

    if (f && stack->size > 0)
    {
        for (size_t i = 0; i < stack->size; i++)
        {
            (*f)(stack->items[i]);
        }
    }

    free(stack->items);
    free(stack);
}

void freeStackAndItems(Stack *stack)
{
    freeStack(stack, free);
}