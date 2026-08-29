#ifndef PARSER_STRUCTS_C_ /* Include guard */
#define PARSER_STRUCTS_C_

#include <stdio.h>
#include <stdbool.h>
#include "../stack/stack.h"

typedef struct HtmlAttribute
{
    char *attributeName;
    char *attributeValue;
} HtmlAttribute;

typedef struct HtmlTag
{
    int tagCount;
    int attributeCount;
    size_t maxSize;
    char *tagName;
    char *content;
    HtmlAttribute **attributes;
    struct HtmlTag **children;
} HtmlTag;

typedef struct ParseState
{
    bool inTag;
    bool tagAdded;
    bool attributeNameAdded;
    bool nonWhiteSpaceInTextContent;
    bool inIgnoredTag;
    size_t maxSize;
    int currentIndex;
    Stack *htmlTags;
    char lastChar;
    char attributeValueMark;
    char *currentItem;
} ParseState;

#endif // PARSER_STRUCTS_C_
