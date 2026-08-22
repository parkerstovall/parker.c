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
        HtmlTag *tag = html_doc->htmlTags[i];
        if (!tag)
        {
            continue;
        }

        for (int j = 0; j < tag->attributeCount; j++)
        {
            HtmlAttribute *attr = tag->attributes[j];
            free(attr->attributeName);
            if (attr->attributeValue)
            {
                free(attr->attributeValue);
            }

            free(attr);
        }

        free(tag->tagName);
        free(tag);
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
    HtmlAttribute *attr = parse_state->html_doc->htmlTags[parse_state->tag_count]->attributes[parse_state->attribute_count];
    attr->attributeValue = malloc(parse_state->current_index + 1);
    if (attr->attributeValue == NULL)
    {
        printf("Error Code: %d\n", errno);
        perror("attributeName");
        return errno;
    }

    for (int j = 0; j < parse_state->current_index; j++)
    {
        attr->attributeValue[j] = parse_state->current_item[j];
    }

    attr->attributeValue[parse_state->current_index] = '\0';
    parse_state->current_index = 0;
    parse_state->attribute_name_added = false;
    parse_state->attribute_count++;

    return 0;
}

int handle_attribute(ParseState *parse_state)
{
    // NEW WAY WITH STRUCT
    size_t new_tag_size = sizeof(HtmlTag) + ((parse_state->attribute_count + 1) * sizeof(HtmlAttribute *));
    HtmlTag *tag = parse_state->html_doc->htmlTags[parse_state->tag_count];
    HtmlTag *tmpTag = realloc(tag, new_tag_size);
    if (!tmpTag)
    {
        printf("Error Code: %d\n", errno);
        perror("*tmpTag");
        return errno;
    }

    tag = tmpTag;
    parse_state->html_doc->htmlTags[parse_state->tag_count] = tag;
    tag->attributes[parse_state->attribute_count] = malloc(sizeof(HtmlAttribute));
    if (tag->attributes[parse_state->attribute_count] == NULL)
    {
        printf("Error Code: %d\n", errno);
        perror("htmlAttribute");
        return errno;
    }

    HtmlAttribute *attr = tag->attributes[parse_state->attribute_count];
    attr->attributeName = malloc(parse_state->current_index + 1);
    if (attr->attributeName == NULL)
    {
        printf("Error Code: %d\n", errno);
        perror("attributeName");
        return errno;
    }

    for (int j = 0; j < parse_state->current_index; j++)
    {
        attr->attributeName[j] = parse_state->current_item[j];
    }

    attr->attributeName[parse_state->current_index] = '\0';
    tag->attributeCount++;
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

    HtmlTag *tag = parse_state->html_doc->htmlTags[parse_state->tag_count];
    tag = malloc(sizeof(HtmlTag));
    if (tag == NULL)
    {
        printf("Error Code: %d\n", errno);
        perror("htmlTags");
        return errno;
    }

    tag->attributeCount = 0;
    parse_state->html_doc->htmlTags[parse_state->tag_count] = tag;
    tag->tagName = malloc(parse_state->current_index + 1);
    if (tag->tagName == NULL)
    {
        printf("Error Code: %d\n", errno);
        perror("tagName");
        return errno;
    }

    for (int j = 0; j < parse_state->current_index; j++)
    {
        tag->tagName[j] = parse_state->current_item[j];
    }

    tag->tagName[parse_state->current_index] = '\0';
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
