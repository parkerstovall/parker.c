#ifndef PARSER_STRUCTS_C_ /* Include guard */
#define PARSER_STRUCTS_C_

#include <stdio.h>
#include <stdbool.h>

typedef struct HtmlAttribute
{
    char *attributeName;
    char *attributeValue;
} HtmlAttribute;

typedef struct HtmlTag
{
    int tagCount;
    int attributeCount;
    char *tagName;
    HtmlAttribute **attributes;
    struct HtmlTag **nestedTags;
} HtmlTag;

typedef struct HtmlDoc
{
    int size;
    HtmlTag *htmlTags[];
} HtmlDoc;

typedef struct ParseState
{
    bool in_tag;
    bool tag_added;
    bool attribute_name_added;
    bool just_opened_tag;
    size_t max_size;
    int current_index;
    int tag_count;
    int attribute_count;
    HtmlDoc *html_doc;
    char *current_item;
} ParseState;

#endif // PARSER_STRUCTS_C_
