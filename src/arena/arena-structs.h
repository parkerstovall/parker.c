#ifndef ARENA_STRUCTS_C /* Include guard */
#define ARENA_STRUCTS_C

#include <stdio.h>

typedef struct Arena
{
    size_t size;
    size_t capacity;
    void *content;
} Arena;

#endif // ARENA_STRUCTS_C