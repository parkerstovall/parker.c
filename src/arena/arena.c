#include "arena-structs.h"
#include <errno.h>
#include <stdlib.h>
#include <string.h>

Arena *newArena(int capacity)
{
    if (capacity <= 0)
    {
        perror("newArena -> all arguments should be higher than zero");
        exit(EXIT_FAILURE);
    }

    Arena *arena = malloc(sizeof(Arena));
    if (!arena)
    {
        printf("Error Code: %d\n", errno);
        perror("*arena");
        exit(EXIT_FAILURE);
    }

    arena->content = malloc((size_t)capacity);
    if (!arena->content)
    {
        printf("Error Code: %d\n", errno);
        perror("arena->content");
        exit(EXIT_FAILURE);
    }
    arena->capacity = (size_t)capacity;
    arena->size = 0;
    return arena;
}

void *arenaAllocate(Arena *arena, size_t requested)
{
    // First check if we have enough space
    size_t requestedSize = arena->size + requested;
    if (requestedSize <= arena->capacity)
    {
        void *returnPtr = arena->content + arena->size;
        arena->size = requestedSize;
        return returnPtr;
    }

    // Otherwise, double arena size and THEN allocate
    size_t newSize = arena->capacity * 2;
    while (newSize <= requestedSize)
    {
        newSize *= 2;
    }

    void *newContent = realloc(arena->content, newSize);
    if (!newContent)
    {
        printf("Error Code: %d\n", errno);
        perror("*newContent");
        exit(EXIT_FAILURE);
    }

    arena->content = newContent;
    arena->capacity = newSize;

    void *returnPtr = arena->content + arena->size;
    arena->size = requestedSize;
    return returnPtr;
}

void *areanReallocate(Arena *arena, void *oldPtr, size_t oldSize, size_t newSize)
{
    void *newPtr = arenaAllocate(arena, newSize);
    if (!newPtr)
    {
        printf("Error Code: %d\n", errno);
        perror("*newPtr");
        exit(EXIT_FAILURE);
    }

    memcpy(newPtr, oldPtr, oldSize);
    return newPtr;
}

void freeArena(Arena *arena)
{
    free(arena->content);
    free(arena);
}