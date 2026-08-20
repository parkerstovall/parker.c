#ifndef PARSER_STRUCTS_C_ /* Include guard */
#define PARSER_STRUCTS_C_

#include <stdio.h>
#include <stdbool.h>

typedef struct
{
    char *attributeName;
    char *attributeValue;
} HtmlAttribute;

typedef struct
{
    int size;
    char *tagName;
    HtmlAttribute *attributes[];
} HtmlTag;

typedef struct
{
    int size;
    HtmlTag *htmlTags[];
} HtmlDoc;

typedef struct
{
    bool in_tag;
    bool tag_added;
    bool attribute_name_added;
    bool just_opened_tag;
    int current_index;
    int tag_count;
    int attribute_count;
    HtmlDoc *html_doc;
    char *current_item;
    size_t max_size;
} ParseState;

#endif // PARSER_STRUCTS_C_
