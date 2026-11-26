#include "../libs/unity/unity.h"
#include "../src/arena/arena-structs.h"
#include "../src/arena/arena.h"

void test_Arena_CanCreateArena(void)
{
    size_t expected = 100;
    Arena *arena = newArena(expected);
    TEST_ASSERT_EQUAL_size_t(arena->capacity, expected);
    TEST_ASSERT_EQUAL_size_t(arena->size, 0);
}

void test_Arena_ReturnsNullWithBadCapacityParameter(void)
{
    Arena *arena = newArena(-1);
    TEST_ASSERT_NULL(arena);
}

void test_Arena_AllocingWillUpdateSize(void)
{
    Arena *arena = newArena(100);
    void *ptr = (arenaAllocate(arena, 50)).ptr;
    TEST_ASSERT_NOT_NULL(ptr);
    TEST_ASSERT_EQUAL_size_t(50, arena->size);
}

void test_Arena_AllocingWillUpdateCapacityIfNeeded(void)
{
    Arena *arena = newArena(75);
    ArenaAllocateResp resp = arenaAllocate(arena, 100);
    Arena *newArena = resp.arena;
    TEST_ASSERT_NOT_NULL(resp.ptr);
    TEST_ASSERT_EQUAL_size_t(0, newArena->size);
    TEST_ASSERT_EQUAL_size_t(150, newArena->capacity);
}

void test_Arena_AllocingWillUpdateCapacityIfNeededMultipleTimes(void)
{
    Arena *arena = newArena(50);
    ArenaAllocateResp resp = arenaAllocate(arena, 101);
    Arena *newArena = resp.arena;
    TEST_ASSERT_NOT_NULL(resp.ptr);
    TEST_ASSERT_EQUAL_size_t(0, newArena->size);
    TEST_ASSERT_EQUAL_size_t(200, newArena->capacity);
}

void test_Arena_ReallocateWorks(void)
{
    Arena *arena = newArena(100);
    int *ptr = (arenaAllocate(arena, sizeof(int) * 3)).ptr;
    ptr[0] = 1;
    ptr[1] = 2;
    ptr[2] = 3;

    int *ptr2 = (areanReallocate(arena, ptr, sizeof(int) * 3, sizeof(int) * 4)).ptr;
    TEST_ASSERT_EQUAL_INT(1, ptr2[0]);
    TEST_ASSERT_EQUAL_INT(2, ptr2[1]);
    TEST_ASSERT_EQUAL_INT(3, ptr2[2]);
}

void test_Arena_ReallocateChangesSize(void)
{
    Arena *arena = newArena(100);
    size_t oldSize = sizeof(int) * 3;
    size_t newSize = sizeof(int) * 4;
    int *ptr = (arenaAllocate(arena, oldSize)).ptr;
    ptr[0] = 1;
    ptr[1] = 2;
    ptr[2] = 3;

    TEST_ASSERT_EQUAL_size_t(oldSize, arena->size);
    areanReallocate(arena, ptr, oldSize, newSize);
    TEST_ASSERT_EQUAL_size_t(oldSize + newSize, arena->size);
}

void test_Arena_AllocationCreatesNewArenaWithParent(void)
{
    Arena *arena = newArena(75);
    ArenaAllocateResp resp = arenaAllocate(arena, 100);
    Arena *newArena = resp.arena;
    TEST_ASSERT_NOT_EQUAL_PTR(arena->content, newArena->content);
    TEST_ASSERT_NOT_EQUAL_PTR(arena, newArena);
    TEST_ASSERT_EQUAL_PTR(arena, newArena->parent);
}

void run_arena_tests(void)
{
    RUN_TEST(test_Arena_CanCreateArena);
    RUN_TEST(test_Arena_ReturnsNullWithBadCapacityParameter);
    RUN_TEST(test_Arena_AllocingWillUpdateSize);
    RUN_TEST(test_Arena_AllocingWillUpdateCapacityIfNeeded);
    RUN_TEST(test_Arena_AllocingWillUpdateCapacityIfNeededMultipleTimes);
    RUN_TEST(test_Arena_ReallocateWorks);
    RUN_TEST(test_Arena_ReallocateChangesSize);
    RUN_TEST(test_Arena_AllocationCreatesNewArenaWithParent);
}