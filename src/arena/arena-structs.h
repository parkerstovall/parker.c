#ifndef ARENA_STRUCTS_C /* Include guard */
#define ARENA_STRUCTS_C

#include <stdio.h>

typedef struct Arena
{
    size_t size;
    size_t capacity;
    void *parent;
    void *content;
} Arena;

typedef struct ArenaAllocateResp
{
    void *arena;
    void *ptr;
} ArenaAllocateResp;

#endif // ARENA_STRUCTS_C