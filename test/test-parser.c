#include "../libs/unity/unity.h"
#include "../src/parser/parser.h"
#include "../src/parser/parser-structs.h"
#include "../src/parser/parser-utils.h"
#include "../src/utils/stack.h"
#include <string.h>

static char ultra_basic_html[] = "<html></html>";
static char ultra_basic_html_with_text[] = "<html>Test</html>";
static char more_complicated_html[] = "<html><a href=\"https://www.example.com\"><i attr1=\"test1\" attr2=\"test2\"></i></a></html>";
// static char example_com_html[] = "<!doctype html><html lang=\"en\"><head><title>Example Domain</title><link rel=\"icon\" href=\"data:,\"><meta name=\"viewport\" content=\"width=device-width, initial-scale=1\"><style>CSS</style></head><body><div><h1>Example Domain</h1><p>This domain is for use in documentation examples without needing permission. Avoid use in operations.</p><p><a href=\"https://iana.org/domains/example\">Learn more</a></p></div></body></html>";

static int getTotalTags(HtmlTag *tag)
{
    int totalTags = 1; // This tag counts
    for (int i = 0; i < tag->tagCount; i++)
    {
        totalTags += getTotalTags(tag->children[i]);
    }

    return totalTags;
}

void test_Parser_OneTagExpected(void)
{
    ParseState *parseState = newParseState();
    parseResponse(ultra_basic_html, strlen(ultra_basic_html), 1, parseState);
    HtmlTag *tag = popStack(parseState->htmlTags, false);

    TEST_ASSERT_EQUAL_CHAR_ARRAY_MESSAGE("ROOT", tag->tagName, strlen("ROOT"), "base tag name should always be 'root'");
    TEST_ASSERT_EQUAL_INT_MESSAGE(1, tag->tagCount, "ROOT tag should have one child");
    TEST_ASSERT_EQUAL_CHAR_ARRAY_MESSAGE("html", tag->children[0]->tagName, strlen("html"), "child tag name should be 'html'");
    TEST_ASSERT_EQUAL_INT_MESSAGE(0, tag->children[0]->tagCount, "HTML tag should not have a child");
}

void test_Parser_OneHtmlTagExpected(void)
{
    ParseState *parseState = newParseState();
    parseResponse(more_complicated_html, strlen(more_complicated_html), 1, parseState);
    HtmlTag *tag = popStack(parseState->htmlTags, false);

    TEST_ASSERT_EQUAL_CHAR_ARRAY_MESSAGE("ROOT", tag->tagName, strlen("ROOT"), "base tag name should always be 'ROOT'");
    TEST_ASSERT_EQUAL_INT_MESSAGE(1, tag->tagCount, "ROOT tag should have one child");
    TEST_ASSERT_EQUAL_CHAR_ARRAY_MESSAGE("html", tag->children[0]->tagName, strlen("html"), "child tag name should be 'html'");
    TEST_ASSERT_EQUAL_INT_MESSAGE(1, tag->children[0]->tagCount, "HTML tag should have one child");
}

void test_Parser_ThreeTagsExpected(void)
{
    int expected = 4;
    ParseState *parseState = newParseState();
    parseResponse(more_complicated_html, strlen(more_complicated_html), 1, parseState);
    HtmlTag *tag = popStack(parseState->htmlTags, false);

    TEST_ASSERT_EQUAL_INT_MESSAGE(expected, getTotalTags(tag), "Three tags expected, plus ROOT tag");
}

void test_Parser_ThreeTagsButLastIsITagExpected(void)
{
    char expected[] = "i";
    ParseState *parseState = newParseState();
    parseResponse(more_complicated_html, strlen(more_complicated_html), 1, parseState);
    HtmlTag *tag = popStack(parseState->htmlTags, false);
    HtmlTag *itag = tag->children[0]->children[0]->children[0];

    TEST_ASSERT_EQUAL_CHAR_ARRAY_MESSAGE(expected, itag->tagName, strlen(expected), "I tag should be the third nested tag");
    TEST_ASSERT_EQUAL_INT_MESSAGE(0, itag->tagCount, "I Tag should have no children");
}

void test_Parser_FindsOneAttribute(void)
{
    int expected = 1;
    ParseState *parseState = newParseState();
    parseResponse(more_complicated_html, strlen(more_complicated_html), 1, parseState);
    HtmlTag *tag = popStack(parseState->htmlTags, false);

    TEST_ASSERT_EQUAL_INT(expected, tag->children[0]->children[0]->attributeCount);
}

void test_Parser_AttributeNameIsCorrect(void)
{
    char expected[] = "href";
    ParseState *parseState = newParseState();
    parseResponse(more_complicated_html, strlen(more_complicated_html), 1, parseState);
    HtmlTag *tag = popStack(parseState->htmlTags, false);

    TEST_ASSERT_EQUAL_CHAR_ARRAY(expected, tag->children[0]->children[0]->attributes[0]->attributeName, strlen(expected));
}

void test_Parser_AttributeValueIsCorrect(void)
{
    char expected[] = "https://www.example.com";
    ParseState *parseState = newParseState();
    parseResponse(more_complicated_html, strlen(more_complicated_html), 1, parseState);
    HtmlTag *tag = popStack(parseState->htmlTags, false);

    TEST_ASSERT_EQUAL_CHAR_ARRAY(expected, tag->children[0]->children[0]->attributes[0]->attributeValue, strlen(expected));
}

void test_Parser_FindsTwoAttributes(void)
{
    int expected = 2;
    ParseState *parseState = newParseState();
    parseResponse(more_complicated_html, strlen(more_complicated_html), 1, parseState);
    HtmlTag *tag = popStack(parseState->htmlTags, false);

    TEST_ASSERT_EQUAL_INT(expected, tag->children[0]->children[0]->children[0]->attributeCount);
}

void test_Parser_FirstAttributeNameIsCorrectForSecondTag(void)
{
    char expected[] = "attr1";

    ParseState *parseState = newParseState();
    parseResponse(more_complicated_html, strlen(more_complicated_html), 1, parseState);
    HtmlTag *tag = popStack(parseState->htmlTags, false);

    TEST_ASSERT_EQUAL_CHAR_ARRAY(expected, tag->children[0]->children[0]->children[0]->attributes[0]->attributeName, strlen(expected));
}

void test_Parser_FirstAttributeValueIsCorrectForSecondTag(void)
{
    char expected[] = "test1";

    ParseState *parseState = newParseState();
    parseResponse(more_complicated_html, strlen(more_complicated_html), 1, parseState);
    HtmlTag *tag = popStack(parseState->htmlTags, false);

    TEST_ASSERT_EQUAL_CHAR_ARRAY(expected, tag->children[0]->children[0]->children[0]->attributes[0]->attributeValue, strlen(expected));
}

void test_Parser_SecondAttributeNameIsCorrectForSecondTag(void)
{
    char expected[] = "attr2";

    ParseState *parseState = newParseState();
    parseResponse(more_complicated_html, strlen(more_complicated_html), 1, parseState);
    HtmlTag *tag = popStack(parseState->htmlTags, false);

    TEST_ASSERT_EQUAL_CHAR_ARRAY(expected, tag->children[0]->children[0]->children[0]->attributes[1]->attributeName, strlen(expected));
}

void test_Parser_SecondAttributeValueIsCorrectForSecondTag(void)
{
    char expected[] = "test2";

    ParseState *parseState = newParseState();
    parseResponse(more_complicated_html, strlen(more_complicated_html), 1, parseState);
    HtmlTag *tag = popStack(parseState->htmlTags, false);

    TEST_ASSERT_EQUAL_CHAR_ARRAY(expected, tag->children[0]->children[0]->children[0]->attributes[1]->attributeValue, strlen(expected));
}

void test_Parser_DoesNotAddChildrenToMetaTags(void)
{
    char *html = "<html><meta><p></p></html>";
    ParseState *parseState = newParseState();
    parseResponse(html, strlen(html), 1, parseState);
    HtmlTag *tag = popStack(parseState->htmlTags, false);

    TEST_ASSERT_EQUAL_INT(1, tag->tagCount);
    TEST_ASSERT_EQUAL_INT(2, tag->children[0]->tagCount);
}

void test_Parser_DoesNotAddChildrenToMetaTagsCaseInsensitive(void)
{
    char *html = "<html><META><p></p></html>";
    ParseState *parseState = newParseState();
    parseResponse(html, strlen(html), 1, parseState);
    HtmlTag *tag = popStack(parseState->htmlTags, false);

    TEST_ASSERT_EQUAL_INT(1, tag->tagCount);
    TEST_ASSERT_EQUAL_INT(2, tag->children[0]->tagCount);
}

void test_Parser_DoesNotAddChildrenToMetaTagsCaseInsensitiveWithAttributes(void)
{
    char *html = "<html><META test=\"123\"><p></p></html>";
    ParseState *parseState = newParseState();
    parseResponse(html, strlen(html), 1, parseState);
    HtmlTag *tag = popStack(parseState->htmlTags, false);

    TEST_ASSERT_EQUAL_INT(1, tag->tagCount);
    TEST_ASSERT_EQUAL_INT(2, tag->children[0]->tagCount);
}

void test_Parser_DoesNotAddAttributeValueIfNotPresent(void)
{
    char *html = "<html><META test><p></p></html>";
    char *expected = "test";
    ParseState *parseState = newParseState();
    parseResponse(html, strlen(html), 1, parseState);
    HtmlTag *tag = popStack(parseState->htmlTags, false);

    TEST_ASSERT_EQUAL_INT(1, tag->children[0]->children[0]->attributeCount);
    TEST_ASSERT_EQUAL_CHAR_ARRAY(expected, tag->children[0]->children[0]->attributes[0]->attributeName, strlen(expected));
    TEST_ASSERT_NULL(tag->children[0]->children[0]->attributes[0]->attributeValue);
}

void test_Parser_DoesNotAddAttributeValueIfNotPresentMiddleOfTag(void)
{
    char *html = "<html><META test attr1=\"value1\"><p></p></html>";
    char *expected = "test";
    ParseState *parseState = newParseState();
    parseResponse(html, strlen(html), 1, parseState);
    HtmlTag *tag = popStack(parseState->htmlTags, false);

    TEST_ASSERT_EQUAL_CHAR_ARRAY(expected, tag->children[0]->children[0]->attributes[0]->attributeName, strlen(expected));
    TEST_ASSERT_NULL(tag->children[0]->children[0]->attributes[0]->attributeValue);
}

void test_Parser_DoesNotAddAttributeValueIfEmpty(void)
{
    char *html = "<html><META test=\"\"><p></p></html>";
    char *expected = "test";
    ParseState *parseState = newParseState();
    parseResponse(html, strlen(html), 1, parseState);
    HtmlTag *tag = popStack(parseState->htmlTags, false);

    TEST_ASSERT_EQUAL_CHAR_ARRAY(expected, tag->children[0]->children[0]->attributes[0]->attributeName, strlen(expected));
    TEST_ASSERT_NULL(tag->children[0]->children[0]->attributes[0]->attributeValue);
}

void test_Parser_FindsTextNotes(void)
{
    char *expected = "Test";
    ParseState *parseState = newParseState();
    parseResponse(ultra_basic_html_with_text, strlen(ultra_basic_html_with_text), 1, parseState);
    HtmlTag *tag = popStack(parseState->htmlTags, false);

    TEST_ASSERT_EQUAL_CHAR_ARRAY("TEXT", tag->children[0]->children[0]->tagName, strlen("TEXT"));
    TEST_ASSERT_EQUAL_CHAR_ARRAY("CONTENT", tag->children[0]->children[0]->attributes[0]->attributeName, strlen("CONTENT"));
    TEST_ASSERT_EQUAL_CHAR_ARRAY(expected, tag->children[0]->children[0]->attributes[0]->attributeValue, strlen(expected));
}

void test_Parser_CanUseEitherSingleOrDoubleQuotesForAttrValues(void)
{
    char *html = "<html attr1=\"value1\" attr2='value2'></html>";
    char *expected1 = "value1";
    char *expected2 = "value2";
    ParseState *parseState = newParseState();
    parseResponse(html, strlen(html), 1, parseState);
    HtmlTag *tag = popStack(parseState->htmlTags, false);

    TEST_ASSERT_EQUAL_INT(2, tag->children[0]->attributeCount);
    TEST_ASSERT_EQUAL_CHAR_ARRAY(expected1, tag->children[0]->attributes[0]->attributeValue, strlen(expected1));
    TEST_ASSERT_EQUAL_CHAR_ARRAY(expected2, tag->children[0]->attributes[1]->attributeValue, strlen(expected2));
}

void test_Parser_ScriptAndStyleTagsIgnored(void)
{
    char *html = "<html attr1=\"value1\" attr2='value2'><script>this is ignored</script><style>This is ignored</style></html>";
    int expected = 0;
    ParseState *parseState = newParseState();
    parseResponse(html, strlen(html), 1, parseState);
    HtmlTag *tag = popStack(parseState->htmlTags, false);

    TEST_ASSERT_EQUAL_INT(expected, tag->children[0]->tagCount);
}

void run_parser_tests()
{
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
    RUN_TEST(test_Parser_SecondAttributeNameIsCorrectForSecondTag);
    RUN_TEST(test_Parser_SecondAttributeValueIsCorrectForSecondTag);
    RUN_TEST(test_Parser_DoesNotAddChildrenToMetaTags);
    RUN_TEST(test_Parser_DoesNotAddChildrenToMetaTagsCaseInsensitive);
    RUN_TEST(test_Parser_DoesNotAddChildrenToMetaTagsCaseInsensitiveWithAttributes);
    RUN_TEST(test_Parser_DoesNotAddAttributeValueIfNotPresent);
    RUN_TEST(test_Parser_DoesNotAddAttributeValueIfNotPresentMiddleOfTag);
    RUN_TEST(test_Parser_DoesNotAddAttributeValueIfEmpty);
    RUN_TEST(test_Parser_FindsTextNotes);
    RUN_TEST(test_Parser_CanUseEitherSingleOrDoubleQuotesForAttrValues);
    RUN_TEST(test_Parser_ScriptAndStyleTagsIgnored);
}