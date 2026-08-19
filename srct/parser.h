#ifndef PARSER_C_ /* Include guard */
#define PARSER_C_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <curl/curl.h>
#include <stdbool.h>
#include <errno.h>

struct HtmlAttribute
{
    char *attributeName;
    char *attributeValue;
};

struct HtmlTag
{
    int size;
    char *tagName;
    struct HtmlAttribute *attributes[];
};

struct HtmlDoc
{
    int size;
    struct HtmlTag *htmlTags[];
};

struct ParseState
{
    bool in_tag;
    bool tag_added;
    bool attribute_name_added;
    bool just_opened_tag;
    int current_index;
    int tag_count;
    int attribute_count;
    struct HtmlDoc *html_doc;
    char *current_item;
    size_t max_size;
};

void free_parse_state(struct ParseState *parse_state, bool freeDoc);

void free_html_doc(struct HtmlDoc *html_doc);

struct ParseState *get_parse_state();

struct HtmlDoc *parse_html(char *url);

size_t parse_response(char *contents, size_t size, size_t nmemb, void *userp);

#endif // PARSER_C_
