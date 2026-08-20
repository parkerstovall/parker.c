#include "parser-structs.h"
#include <errno.h>
#include <stdlib.h>

void free_html_doc(HtmlDoc *html_doc)
{
    if (!html_doc)
    {
        return;
    }

    for (int i = 0; i < html_doc->size; i++)
    {
        if (!html_doc->htmlTags[i])
        {
            continue;
        }

        for (int j = 0; j < html_doc->htmlTags[i]->size; j++)
        {
            free(html_doc->htmlTags[i]->attributes[j]->attributeName);
            if (html_doc->htmlTags[i]->attributes[j]->attributeValue)
            {
                free(html_doc->htmlTags[i]->attributes[j]->attributeValue);
            }

            free(html_doc->htmlTags[i]->attributes[j]);
        }

        free(html_doc->htmlTags[i]->tagName);
        free(html_doc->htmlTags[i]);
    }

    free(html_doc);
}

void free_parse_state(ParseState *parse_state, bool freeDoc)
{
    if (!parse_state)
    {
        return;
    }

    if (freeDoc)
    {
        free_html_doc(parse_state->html_doc);
    }

    if (parse_state->current_item)
    {
        free(parse_state->current_item);
    }

    free(parse_state);
}

int handle_attribute_value(ParseState *parse_state)
{
    parse_state->html_doc->htmlTags[parse_state->tag_count]->attributes[parse_state->attribute_count]->attributeValue = malloc(parse_state->current_index + 1);
    if (parse_state->html_doc->htmlTags[parse_state->tag_count]->attributes[parse_state->attribute_count]->attributeValue == NULL)
    {
        printf("Error Code: %d\n", errno);
        perror("attributeName");
        return errno;
    }

    for (int j = 0; j < parse_state->current_index; j++)
    {
        parse_state->html_doc->htmlTags[parse_state->tag_count]->attributes[parse_state->attribute_count]->attributeValue[j] = parse_state->current_item[j];
    }

    parse_state->html_doc->htmlTags[parse_state->tag_count]->attributes[parse_state->attribute_count]->attributeValue[parse_state->current_index] = '\0';
    parse_state->current_index = 0;
    parse_state->attribute_name_added = false;
    parse_state->attribute_count++;

    return 0;
}

int handle_attribute(ParseState *parse_state)
{
    // NEW WAY WITH STRUCT
    size_t new_tag_size = sizeof(HtmlTag) + ((parse_state->attribute_count + 1) * sizeof(HtmlAttribute *));
    HtmlTag *tmpTag = realloc(parse_state->html_doc->htmlTags[parse_state->tag_count], new_tag_size);
    if (!tmpTag)
    {
        printf("Error Code: %d\n", errno);
        perror("*tmpTag");
        return errno;
    }

    parse_state->html_doc->htmlTags[parse_state->tag_count] = tmpTag;
    parse_state->html_doc->htmlTags[parse_state->tag_count]->attributes[parse_state->attribute_count] = malloc(sizeof(HtmlAttribute));
    if (parse_state->html_doc->htmlTags[parse_state->tag_count]->attributes[parse_state->attribute_count] == NULL)
    {
        printf("Error Code: %d\n", errno);
        perror("htmlAttribute");
        return errno;
    }

    parse_state->html_doc->htmlTags[parse_state->tag_count]->attributes[parse_state->attribute_count]->attributeName = malloc(parse_state->current_index + 1);
    if (parse_state->html_doc->htmlTags[parse_state->tag_count]->attributes[parse_state->attribute_count]->attributeName == NULL)
    {
        printf("Error Code: %d\n", errno);
        perror("attributeName");
        return errno;
    }

    for (int j = 0; j < parse_state->current_index; j++)
    {
        parse_state->html_doc->htmlTags[parse_state->tag_count]->attributes[parse_state->attribute_count]->attributeName[j] = parse_state->current_item[j];
    }

    parse_state->html_doc->htmlTags[parse_state->tag_count]->attributes[parse_state->attribute_count]->attributeName[parse_state->current_index] = '\0';
    parse_state->html_doc->htmlTags[parse_state->tag_count]->size++;
    parse_state->current_index = 0;
    parse_state->attribute_name_added = true;

    return 0;
}

int handle_item_break(ParseState *parse_state)
{
    // NEW WAY WITH STRUCT
    size_t new_doc_size = sizeof(HtmlDoc) + ((parse_state->tag_count + 1) * sizeof(HtmlTag *));
    HtmlDoc *tmpDoc = realloc(parse_state->html_doc, new_doc_size);
    if (tmpDoc == NULL)
    {
        printf("Error Code: %d\n", errno);
        perror("*tmpDoc");
        return errno;
    }

    parse_state->html_doc = tmpDoc;
    parse_state->html_doc->size++;

    parse_state->html_doc->htmlTags[parse_state->tag_count] = malloc(sizeof(HtmlTag));
    if (parse_state->html_doc->htmlTags[parse_state->tag_count] == NULL)
    {
        printf("Error Code: %d\n", errno);
        perror("htmlTags");
        return errno;
    }

    parse_state->html_doc->htmlTags[parse_state->tag_count]->size = 0;
    parse_state->html_doc->htmlTags[parse_state->tag_count]->tagName = malloc(parse_state->current_index + 1);
    if (parse_state->html_doc->htmlTags[parse_state->tag_count]->tagName == NULL)
    {
        printf("Error Code: %d\n", errno);
        perror("tagName");
        return errno;
    }

    for (int j = 0; j < parse_state->current_index; j++)
    {
        parse_state->html_doc->htmlTags[parse_state->tag_count]->tagName[j] = parse_state->current_item[j];
    }

    parse_state->html_doc->htmlTags[parse_state->tag_count]->tagName[parse_state->current_index] = '\0';
    parse_state->attribute_count = 0;
    parse_state->current_index = 0;

    return 0;
}

int append_char_to_item(ParseState *parse_state, char c)
{
    size_t new_size = sizeof(char) * parse_state->current_index;
    if (new_size >= parse_state->max_size)
    {
        new_size = parse_state->max_size * 2;
        void *tmp = realloc(parse_state->current_item, new_size);
        parse_state->max_size = new_size;
        if (!tmp)
        {
            printf("Error Code: %d\n", errno);
            perror("*tmp");
            return errno;
        }

        parse_state->current_item = tmp;
    }

    parse_state->current_item[parse_state->current_index++] = c;

    return 0;
}
