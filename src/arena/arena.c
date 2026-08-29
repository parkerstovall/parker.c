#include "arena-structs.h"
#include <errno.h>
#include <stdlib.h>
#include <string.h>

Arena *newArena(int capacity)
{
    if (capacity <= 0)
    {
        perror("newArena -> all arguments should be higher than zero");
        return NULL;
    }

    Arena *arena = malloc(sizeof(Arena));
    arena->content = malloc((size_t)capacity);
    arena->capacity = (size_t)capacity;
    arena->size = 0;
    return arena;
}

void *arenaAllocate(Arena *arena, size_t requested)
{
    // First check if we have enough space
    size_t requestedSize = arena->size + requested;
    if (requestedSize < arena->capacity)
    {
        void *returnPtr = arena->content + arena->size;
        arena->size = requestedSize;
        return returnPtr;
    }

    // Otherwise, double arena size and THEN allocate
    size_t newSize = arena->capacity * 2;
    while (newSize <= requestedSize)
    {
        newSize += arena->capacity;
    }

    void *newContent = realloc(arena->content, newSize);
    if (!newContent)
    {
        printf("Error Code: %d\n", errno);
        perror("*tmpTags");
        return NULL;
    }

    arena->content = newContent;
    arena->capacity = newSize;

    void *returnPtr = arena->content + arena->size;
    arena->size = requestedSize;
    return returnPtr;
}

void *areanaReallocate(Arena *arena, void *oldPtr, size_t oldSize, size_t newSize)
{
    void *newPtr = arenaAllocate(arena, newSize);
    memcpy(newPtr, oldPtr, oldSize);
    return newPtr;
}

void freeArena(Arena *arena)
{
    free(arena->content);
    free(arena);
}