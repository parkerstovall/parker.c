#ifndef ARENA_C /* Include guard */
#define ARENA_C

#include "arena-structs.h"

Arena *newArena(size_t capacity);

void *arenaAllocate(Arena *arena, size_t requested);

void *areanaReallocate(Arena *arena, void *oldPtr, size_t oldSize, size_t newSize);

void freeArena(Arena *arena);

#endif // ARENA_C