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
    if (!arena)
    {
        printf("Error Code: %d\n", errno);
        perror("*arena");
        return NULL;
    }

    arena->content = malloc((size_t)capacity);
    if (!arena->content)
    {
        printf("Error Code: %d\n", errno);
        perror("arena->content");
        return NULL;
    }
    arena->capacity = (size_t)capacity;
    arena->size = 0;
    arena->parent = NULL;
    return arena;
}

ArenaAllocateResp arenaAllocate(Arena *arena, size_t requested)
{
    ArenaAllocateResp resp;
    resp.arena = arena;

    // First check if we have enough space
    size_t requestedSize = arena->size + requested;
    if (requestedSize <= arena->capacity)
    {
        void *returnPtr = arena->content + arena->size;
        arena->size = requestedSize;
        resp.ptr = returnPtr;
        return resp;
    }

    // Otherwise, double arena size and THEN allocate
    size_t newSize = arena->capacity * 2;
    while (newSize <= requestedSize)
    {
        newSize *= 2;
    }

    Arena *largerArena = newArena(newSize);
    largerArena->parent = arena;

    void *returnPtr = largerArena->content + largerArena->size;
    resp.arena = largerArena;
    resp.ptr = returnPtr;
    return resp;
}

ArenaAllocateResp areanReallocate(Arena *arena, void *oldPtr, size_t oldSize, size_t newSize)
{
    ArenaAllocateResp resp = arenaAllocate(arena, newSize);
    if (!resp.ptr)
    {
        printf("Error Code: %d\n", errno);
        perror("*resp.ptr");
        exit(EXIT_FAILURE);
    }

    memcpy(resp.ptr, oldPtr, oldSize);
    return resp;
}

void freeArena(Arena *arena)
{
    if (arena->parent != NULL)
    {
        freeArena(arena->parent);
    }

    free(arena->content);
    free(arena);
}