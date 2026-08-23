#include "../libs/unity/unity.h"
#include "../src/utils/stack.h"
#include "../src/utils/stack-structs.h"

void test_Stack_DefaultStackUsesDefaultValues(void)
{
    Stack *stack = defaultStack();
    TEST_ASSERT_EQUAL_size_t_MESSAGE(1, stack->capacity, "Default capacity should be 1");
    TEST_ASSERT_EQUAL_size_t_MESSAGE(1, stack->growthStep, "Default growthStep should be 1");
    TEST_ASSERT_EQUAL_size_t_MESSAGE(0, stack->size, "Size is 0 upon creation");
}

void test_Stack_StackSupportsArguments(void)
{
    Stack *stack = newStack(10, 25);
    TEST_ASSERT_EQUAL_size_t_MESSAGE(10, stack->capacity, "capacity should follow param");
    TEST_ASSERT_EQUAL_size_t_MESSAGE(25, stack->growthStep, "growthStep should follow param");
    TEST_ASSERT_EQUAL_size_t_MESSAGE(0, stack->size, "Size is 0 upon creation");
}

void test_Stack_AddItemToStackChangesSize(void)
{
    Stack *stack = defaultStack();
    int x = 1;
    pushStack(stack, &x);
    TEST_ASSERT_EQUAL_size_t_MESSAGE(1, stack->size, "Size should increase on push");
    TEST_ASSERT_EQUAL_size_t_MESSAGE(1, stack->capacity, "Capacity should not increase when not neeed");
}

void test_Stack_AddItemToStackChangesCapacity(void)
{
    Stack *stack = defaultStack();
    int x = 1;
    pushStack(stack, &x);
    pushStack(stack, &x);
    TEST_ASSERT_EQUAL_size_t_MESSAGE(2, stack->size, "Size should increase for each push");
    TEST_ASSERT_EQUAL_size_t_MESSAGE(2, stack->capacity, "Capacity should increase by 1 (default growthStep)");
}

void test_Stack_CapacityIncreasesByGrowthStep(void)
{
    Stack *stack = newStack(1, 5);
    int x = 1;
    pushStack(stack, &x);
    pushStack(stack, &x);
    TEST_ASSERT_EQUAL_size_t_MESSAGE(6, stack->capacity, "Capacity should increase by growthStep");
}

void test_Stack_PopItemReturnsItem(void)
{
    Stack *stack = newStack(1, 5);
    int x = 1;
    int y = 3;
    pushStack(stack, &x);
    pushStack(stack, &y);

    TEST_ASSERT_EQUAL_size_t_MESSAGE(2, stack->size, "Size should increase after adding");
    void *item = popStack(stack, false);
    TEST_ASSERT_EQUAL_size_t_MESSAGE(1, stack->size, "Size should decrease after removing");
    TEST_ASSERT_EQUAL_PTR_MESSAGE(&y, item, "Should return pointer to last item");
}

void test_Stack_PopItemReducesCapacityWhenSpecified(void)
{
    Stack *stack = defaultStack();
    int x = 1;
    pushStack(stack, &x);
    pushStack(stack, &x);

    TEST_ASSERT_EQUAL_size_t_MESSAGE(2, stack->size, "Size should increase after adding");
    TEST_ASSERT_EQUAL_size_t_MESSAGE(2, stack->capacity, "Capacity should increase after adding");
    popStack(stack, true);
    TEST_ASSERT_EQUAL_size_t_MESSAGE(1, stack->size, "Size should decrease after removing");
    TEST_ASSERT_EQUAL_size_t_MESSAGE(1, stack->capacity, "Capacity should decrease after removing");
}

void test_Stack_PopItemDoesNotReduceCapacityWhenSpecified(void)
{
    Stack *stack = defaultStack();
    int x = 1;
    pushStack(stack, &x);
    pushStack(stack, &x);

    TEST_ASSERT_EQUAL_size_t_MESSAGE(2, stack->size, "Size should increase after adding");
    TEST_ASSERT_EQUAL_size_t_MESSAGE(2, stack->capacity, "Capacity should increase after adding");
    popStack(stack, false);
    TEST_ASSERT_EQUAL_size_t_MESSAGE(1, stack->size, "Size should decrease after removing");
    TEST_ASSERT_EQUAL_size_t_MESSAGE(2, stack->capacity, "Capacity should not decrease after removing if requested");
}

void test_Stack_PeekReturnsTopItem(void)
{
    Stack *stack = defaultStack();
    int x = 1;
    pushStack(stack, &x);

    void *peek = peekStack(stack);

    TEST_ASSERT_EQUAL_size_t_MESSAGE(1, stack->size, "Peek should not change size");
    TEST_ASSERT_EQUAL_PTR_MESSAGE(&x, peek, "Peek should return top item");
}

void test_Stack_NegativeGrowthStepReturnsNull(void)
{
    Stack *stack = newStack(1, -1);
    TEST_ASSERT_NULL_MESSAGE(stack, "Negative growthStep should fail stack creation");
}

void test_Stack_NegativeCapacityReturnsNull(void)
{
    Stack *stack = newStack(-1, 1);
    TEST_ASSERT_NULL_MESSAGE(stack, "Negative capacity should fail stack creation");
}

void test_Stack_CapacityDecreasesByGrowthStep(void)
{
    Stack *stack = newStack(1, 5);
    int x = 1;
    pushStack(stack, &x);
    pushStack(stack, &x);
    TEST_ASSERT_EQUAL_size_t_MESSAGE(6, stack->capacity, "Capacity should increase by growthStep");

    pushStack(stack, &x);
    pushStack(stack, &x);
    TEST_ASSERT_EQUAL_size_t_MESSAGE(6, stack->capacity, "Capacity should increase by growthStep");

    popStack(stack, true);
    popStack(stack, true);
    TEST_ASSERT_EQUAL_size_t_MESSAGE(6, stack->capacity, "Capacity should increase by growthStep");

    popStack(stack, true);
    popStack(stack, true);
    TEST_ASSERT_EQUAL_size_t_MESSAGE(1, stack->capacity, "Capacity should increase by growthStep");
}

void run_stack_tests()
{
    RUN_TEST(test_Stack_DefaultStackUsesDefaultValues);
    RUN_TEST(test_Stack_StackSupportsArguments);
    RUN_TEST(test_Stack_AddItemToStackChangesSize);
    RUN_TEST(test_Stack_AddItemToStackChangesCapacity);
    RUN_TEST(test_Stack_CapacityIncreasesByGrowthStep);
    RUN_TEST(test_Stack_PopItemReturnsItem);
    RUN_TEST(test_Stack_PopItemReducesCapacityWhenSpecified);
    RUN_TEST(test_Stack_PopItemDoesNotReduceCapacityWhenSpecified);
    RUN_TEST(test_Stack_PeekReturnsTopItem);
    RUN_TEST(test_Stack_NegativeGrowthStepReturnsNull);
    RUN_TEST(test_Stack_NegativeCapacityReturnsNull);
    RUN_TEST(test_Stack_CapacityDecreasesByGrowthStep);
}