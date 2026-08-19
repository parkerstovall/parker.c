#include "../libs/unity/unity.h"
#include "../src/parser.h"
#include <string.h>

void setUp() {}    // Nothing to do yet
void tearDown() {} // Nothing to do yet

static char ultra_basic_html[] = "<html>Testing</html>";
static char more_complicated_html[] = "<html>Testing<a href=\"https://www.example.com\">Link! <i attr1=\"test1\" attr2=\"test2\">with italics</i></a></html>";

void test_Parser_OneTagExpected(void)
{
    int expected = 1;
    struct ParseState *parse_state = get_parse_state();

    parse_response(ultra_basic_html, sizeof(ultra_basic_html), 1, parse_state);
    TEST_ASSERT_EQUAL_INT(expected, parse_state->html_doc->size);
}

void test_Parser_OneHtmlTagExpected(void)
{
    char expected[] = "html";
    struct ParseState *parse_state = get_parse_state();

    parse_response(ultra_basic_html, sizeof(ultra_basic_html), 1, parse_state);
    TEST_ASSERT_EQUAL_CHAR_ARRAY(expected, parse_state->html_doc->htmlTags[0]->tagName, strlen(expected));
}

void test_Parser_ThreeTagsExpected(void)
{
    int expected = 3;
    struct ParseState *parse_state = get_parse_state();

    parse_response(more_complicated_html, sizeof(more_complicated_html), 1, parse_state);
    TEST_ASSERT_EQUAL_INT(expected, parse_state->html_doc->size);
}

void test_Parser_ThreeTagsButLastIsITagExpected(void)
{
    char expected[] = "i";
    struct ParseState *parse_state = get_parse_state();

    parse_response(more_complicated_html, sizeof(more_complicated_html), 1, parse_state);
    TEST_ASSERT_EQUAL_CHAR_ARRAY(expected, parse_state->html_doc->htmlTags[2]->tagName, strlen(expected));
}

void test_Parser_FindsOneAttribute(void)
{
    int expected = 1;
    struct ParseState *parse_state = get_parse_state();

    parse_response(more_complicated_html, sizeof(more_complicated_html), 1, parse_state);
    TEST_ASSERT_EQUAL_INT(expected, parse_state->html_doc->htmlTags[1]->size);
}

void test_Parser_AttributeNameIsCorrect(void)
{
    char expected[] = "href";
    struct ParseState *parse_state = get_parse_state();

    parse_response(more_complicated_html, sizeof(more_complicated_html), 1, parse_state);
    TEST_ASSERT_EQUAL_CHAR_ARRAY(expected, parse_state->html_doc->htmlTags[1]->attributes[0]->attributeName, strlen(expected));
}

void test_Parser_AttributeValueIsCorrect(void)
{
    char expected[] = "https://www.example.com";
    struct ParseState *parse_state = get_parse_state();

    parse_response(more_complicated_html, sizeof(more_complicated_html), 1, parse_state);
    TEST_ASSERT_EQUAL_CHAR_ARRAY(expected, parse_state->html_doc->htmlTags[1]->attributes[0]->attributeValue, strlen(expected));
}

void test_Parser_FindsTwoAttributes(void)
{
    int expected = 2;
    struct ParseState *parse_state = get_parse_state();

    parse_response(more_complicated_html, sizeof(more_complicated_html), 1, parse_state);
    TEST_ASSERT_EQUAL_INT(expected, parse_state->html_doc->htmlTags[2]->size);
}

void test_Parser_FirstAttributeNameIsCorrectForSecondTag(void)
{
    char expected[] = "attr1";
    struct ParseState *parse_state = get_parse_state();

    parse_response(more_complicated_html, sizeof(more_complicated_html), 1, parse_state);
    TEST_ASSERT_EQUAL_CHAR_ARRAY(expected, parse_state->html_doc->htmlTags[2]->attributes[0]->attributeName, strlen(expected));
}

void test_Parser_FirstAttributeValueIsCorrectForSecondTag(void)
{
    char expected[] = "test1";
    struct ParseState *parse_state = get_parse_state();

    parse_response(more_complicated_html, sizeof(more_complicated_html), 1, parse_state);
    TEST_ASSERT_EQUAL_CHAR_ARRAY(expected, parse_state->html_doc->htmlTags[2]->attributes[0]->attributeValue, strlen(expected));
}

void test_Parser_SecondAttributeNameIsCorrectForSecondTag(void)
{
    char expected[] = "attr2";
    struct ParseState *parse_state = get_parse_state();

    parse_response(more_complicated_html, sizeof(more_complicated_html), 1, parse_state);
    TEST_ASSERT_EQUAL_CHAR_ARRAY(expected, parse_state->html_doc->htmlTags[2]->attributes[1]->attributeName, strlen(expected));
}
void test_Parser_SecondAttributeValueIsCorrectForSecondTag(void)
{
    char expected[] = "test2";
    struct ParseState *parse_state = get_parse_state();

    parse_response(more_complicated_html, sizeof(more_complicated_html), 1, parse_state);
    TEST_ASSERT_EQUAL_CHAR_ARRAY(expected, parse_state->html_doc->htmlTags[2]->attributes[1]->attributeValue, strlen(expected));
}

int main(void)
{
    UNITY_BEGIN();
    RUN_TEST(test_Parser_OneTagExpected);
    RUN_TEST(test_Parser_OneHtmlTagExpected);
    RUN_TEST(test_Parser_ThreeTagsExpected);
    RUN_TEST(test_Parser_ThreeTagsButLastIsITagExpected);
    RUN_TEST(test_Parser_FindsOneAttribute);
    RUN_TEST(test_Parser_AttributeNameIsCorrect);
    RUN_TEST(test_Parser_AttributeValueIsCorrect);
    RUN_TEST(test_Parser_FindsTwoAttributes);
    RUN_TEST(test_Parser_FirstAttributeNameIsCorrectForSecondTag);
    RUN_TEST(test_Parser_FirstAttributeValueIsCorrectForSecondTag);
    // RUN_TEST(test_Parser_SecondAttributeNameIsCorrectForSecondTag);
    // RUN_TEST(test_Parser_SecondAttributeValueIsCorrectForSecondTag);
    return UNITY_END();
}