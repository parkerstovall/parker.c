#include "test-parser.h"
#include <string.h>
#include "../libs/unity/unity.h"

void setUp() {}    // Nothing to do yet
void tearDown() {} // Nothing to do yet

int main(void)
{
    UNITY_BEGIN();

    run_parser_tests();

    return UNITY_END();
}