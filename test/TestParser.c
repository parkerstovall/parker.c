#include "../libs/unity/unity.h"
#include "../src/Parser.h"

void setUp() {}    // Nothing to do yet
void tearDown() {} // Nothing to do yet

void test_ParseBasicHtml(void)
{
    char html[] = "<html>Testing</html>";
    int expected = 1;
    struct ParseState *parse_state = get_parse_state();
    parse_response(html, sizeof(html), 1, parse_state);
    TEST_ASSERT_EQUAL_INT(expected, parse_state->html_doc->size);
}

int main(void)
{
    UNITY_BEGIN();
    RUN_TEST(test_ParseBasicHtml);
    return UNITY_END();
}