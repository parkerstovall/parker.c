#include "parser-structs.h"
#include <errno.h>
#include <stdlib.h>
#include <stdio.h>

void freeHtmlTag(HtmlTag *htmlTag)
{
    if (!htmlTag)
    {
        return;
    }

    for (int i = 0; i < htmlTag->tagCount; i++)
    {
        HtmlTag *tag = htmlTag->children[i];
        if (!tag)
        {
            continue;
        }

        freeHtmlTag(tag);
    }

    for (int i = 0; i < htmlTag->attributeCount; i++)
    {
        if (!htmlTag->attributes[i]->attributeName)
        {
            free(htmlTag->attributes[i]->attributeName);
        }

        if (htmlTag->attributes[i]->attributeValue)
        {
            free(htmlTag->attributes[i]->attributeValue);
        }
    }

    if (htmlTag->tagName)
    {
        free(htmlTag->tagName);
    }

    free(htmlTag);
}

void freeParseState(ParseState *parseState, bool freeTags)
{
    if (!parseState)
    {
        return;
    }

    if (freeTags)
    {
        freeStackAndItems(parseState->htmlTags);
    }

    if (parseState->currentItem)
    {
        free(parseState->currentItem);
    }

    free(parseState);
}

int handleAttributeValue(ParseState *parseState)
{
    HtmlTag *tag = peekStack(parseState->htmlTags);
    HtmlAttribute *attr = tag->attributes[tag->attributeCount];

    attr->attributeValue = malloc(parseState->currentIndex + 1);
    if (attr->attributeValue == NULL)
    {
        printf("Error Code: %d\n", errno);
        perror("attributeName");
        return errno;
    }

    for (int j = 0; j < parseState->currentIndex; j++)
    {
        attr->attributeValue[j] = parseState->currentItem[j];
    }

    attr->attributeValue[parseState->currentIndex] = '\0';
    parseState->currentIndex = 0;
    parseState->attributeNameAdded = false;

    return 0;
}

int handleNewAttribute(ParseState *parseState)
{
    HtmlTag *tag = peekStack(parseState->htmlTags);
    if (tag->attributeCount == 0)
    {
        tag->attributes = malloc(sizeof(HtmlAttribute *));
        if (!tag->attributes)
        {
            printf("Error Code: %d\n", errno);
            perror("tag->attributes");
            return errno;
        }
    }
    else
    {
        HtmlAttribute **tmpAttributes = realloc(tag->attributes, sizeof(HtmlAttribute *) * (tag->attributeCount + 1));
        if (!tmpAttributes)
        {
            printf("Error Code: %d\n", errno);
            perror("*tmpAttributes");
            return errno;
        }

        tag->attributes = tmpAttributes;
    }

    tag->attributes[tag->attributeCount] = malloc(sizeof(HtmlAttribute));
    if (tag->attributes[tag->attributeCount] == NULL)
    {
        printf("Error Code: %d\n", errno);
        perror("htmlAttribute");
        return errno;
    }

    HtmlAttribute *attr = tag->attributes[tag->attributeCount];
    attr->attributeName = malloc(parseState->currentIndex + 1);
    if (attr->attributeName == NULL)
    {
        printf("Error Code: %d\n", errno);
        perror("attributeName");
        return errno;
    }

    for (int j = 0; j < parseState->currentIndex; j++)
    {
        attr->attributeName[j] = parseState->currentItem[j];
    }

    attr->attributeName[parseState->currentIndex] = '\0';
    attr->attributeValue = NULL;
    tag->attributeCount++;
    parseState->currentIndex = 0;
    parseState->attributeNameAdded = true;

    return 0;
}

int handleNewTag(ParseState *parseState)
{
    HtmlTag *tag = malloc(sizeof(HtmlTag));
    if (tag == NULL)
    {
        printf("Error Code: %d\n", errno);
        perror("htmlTags");
        return errno;
    }

    tag->children = NULL;
    tag->tagCount = 0;

    HtmlTag *currentTag = peekStack(parseState->htmlTags);
    if (currentTag->tagCount == 0)
    {
        currentTag->children = malloc(sizeof(HtmlTag *));
        if (!currentTag->children)
        {
            printf("Error Code: %d\n", errno);
            perror("currentTag->children");
            return errno;
        }
    }
    else
    {
        HtmlTag **tmpTags = realloc(currentTag->children, sizeof(HtmlTag *) * (currentTag->tagCount + 1));
        if (!tmpTags)
        {
            printf("Error Code: %d\n", errno);
            perror("*tmpTags");
            return errno;
        }

        currentTag->children = tmpTags;
    }

    currentTag->children[currentTag->tagCount] = tag;
    currentTag->tagCount++;

    pushStack(parseState->htmlTags, tag);
    tag->attributeCount = 0;
    tag->tagName = malloc(parseState->currentIndex + 1);
    if (tag->tagName == NULL)
    {
        printf("Error Code: %d\n", errno);
        perror("tagName");
        return errno;
    }

    for (int j = 0; j < parseState->currentIndex; j++)
    {
        tag->tagName[j] = parseState->currentItem[j];
    }

    tag->tagName[parseState->currentIndex] = '\0';
    parseState->currentIndex = 0;

    return 0;
}

int appendCharToItem(ParseState *parseState, char c)
{
    size_t new_size = sizeof(char) * parseState->currentIndex;
    if (new_size >= parseState->maxSize)
    {
        new_size = parseState->maxSize * 2;
        void *tmp = realloc(parseState->currentItem, new_size);
        parseState->maxSize = new_size;
        if (!tmp)
        {
            printf("Error Code: %d\n", errno);
            perror("*tmp");
            return errno;
        }

        parseState->currentItem = tmp;
    }

    parseState->currentItem[parseState->currentIndex++] = c;

    return 0;
}
