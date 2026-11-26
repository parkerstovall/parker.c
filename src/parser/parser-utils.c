#include "parser-structs.h"
#include "../arena/arena.h"
#include <string.h>
#include <errno.h>
#include <stdlib.h>
#include <stdio.h>

void freeParseState(ParseState *parseState, bool freeTags)
{
    if (!parseState)
    {
        return;
    }

    if (freeTags)
    {
        freeStack(parseState->htmlTags, NULL);
    }

    free(parseState);
}

HtmlTag *peekCurrentTag(ParseState *parseState)
{
    size_t *tagLocation = peekStack(parseState->htmlTags);
    return parseState->arena->content + *tagLocation;
}

int handleTextNode(ParseState *parseState)
{
    // Dont want to add whitespace as text content
    if (!parseState->nonWhiteSpaceInTextContent)
    {
        return 0;
    }

    HtmlAttribute *textAttr = arenaAllocate(parseState->arena, sizeof(HtmlAttribute));
    textAttr->attributeName = "CONTENT";

    textAttr->attributeValue = arenaAllocate(parseState->arena, sizeof(char) * parseState->currentIndex + 1);
    if (textAttr->attributeValue == NULL)
    {
        printf("Error Code: %d\n", errno);
        perror("attributeValue");
        exit(EXIT_FAILURE);
    }

    for (int j = 0; j < parseState->currentIndex; j++)
    {
        textAttr->attributeValue[j] = parseState->currentItem[j];
    }

    textAttr->attributeValue[parseState->currentIndex] = '\0';

    HtmlTag *tag = arenaAllocate(parseState->arena, sizeof(HtmlTag));
    if (tag == NULL)
    {
        printf("Error Code: %d\n", errno);
        perror("htmlTags");
        exit(EXIT_FAILURE);
    }

    tag->children = NULL;
    tag->tagCount = 0;
    tag->tagName = "TEXT";

    HtmlTag *currentTag = peekCurrentTag(parseState);
    if (currentTag->tagCount == 0)
    {
        currentTag->children = arenaAllocate(parseState->arena, sizeof(HtmlTag *));
        if (!currentTag->children)
        {
            printf("Error Code: %d\n", errno);
            perror("currentTag->children");
            exit(EXIT_FAILURE);
        }
    }
    else
    {
        size_t newSize = sizeof(HtmlTag *) * (currentTag->tagCount + 1);
        if (newSize > currentTag->maxSize)
        {
            newSize = sizeof(HtmlTag *) * (currentTag->tagCount * 2);
            HtmlTag **tmpTags = areanReallocate(parseState->arena, currentTag->children, currentTag->maxSize, newSize);
            if (!tmpTags)
            {
                printf("Error Code: %d\n", errno);
                perror("*tmpTags");
                exit(EXIT_FAILURE);
            }

            currentTag->children = tmpTags;
            currentTag->maxSize = newSize;
        }
    }

    tag->attributes = arenaAllocate(parseState->arena, sizeof(HtmlAttribute *));
    if (!tag->attributes)
    {
        printf("Error Code: %d\n", errno);
        perror("tag->attributes");
        exit(EXIT_FAILURE);
    }

    tag->attributes[0] = textAttr;
    tag->attributeCount = 1;

    currentTag->children[currentTag->tagCount] = tag;
    currentTag->tagCount++;
    parseState->nonWhiteSpaceInTextContent = false;

    return 0;
}

int handleAttributeValue(ParseState *parseState)
{
    HtmlTag *tag = peekCurrentTag(parseState);
    HtmlAttribute *attr = tag->attributes[tag->attributeCount - 1];
    attr->attributeValue = arenaAllocate(parseState->arena, sizeof(char) * parseState->currentIndex + 1);
    if (attr->attributeValue == NULL)
    {
        printf("Error Code: %d\n", errno);
        perror("attributeName");
        exit(EXIT_FAILURE);
    }

    for (int j = 0; j < parseState->currentIndex; j++)
    {
        attr->attributeValue[j] = parseState->currentItem[j];
    }

    attr->attributeValue[parseState->currentIndex] = '\0';
    parseState->currentIndex = 0;
    parseState->currentItem[0] = '\0';
    parseState->attributeNameAdded = false;
    parseState->attributeValueMark = '\0';
    parseState->nonWhiteSpaceInTextContent = false;

    return 0;
}

int handleNewAttribute(ParseState *parseState)
{
    HtmlTag *tag = peekCurrentTag(parseState);
    if (tag->attributeCount == 0)
    {
        tag->attributes = arenaAllocate(parseState->arena, sizeof(HtmlAttribute *));
        if (!tag->attributes)
        {
            printf("Error Code: %d\n", errno);
            perror("tag->attributes");
            exit(EXIT_FAILURE);
        }
    }
    else
    {
        size_t oldSize = sizeof(HtmlAttribute *) * (tag->attributeCount);
        size_t newSize = sizeof(HtmlAttribute *) * (tag->attributeCount + 1);
        HtmlAttribute **tmpAttributes = areanReallocate(parseState->arena, tag->attributes, oldSize, newSize);
        if (!tmpAttributes)
        {
            printf("Error Code: %d\n", errno);
            perror("*tmpAttributes");
            exit(EXIT_FAILURE);
        }

        tag->attributes = tmpAttributes;
    }

    tag->attributes[tag->attributeCount] = arenaAllocate(parseState->arena, sizeof(HtmlAttribute));
    if (tag->attributes[tag->attributeCount] == NULL)
    {
        printf("Error Code: %d\n", errno);
        perror("htmlAttribute");
        exit(EXIT_FAILURE);
    }

    HtmlAttribute *attr = tag->attributes[tag->attributeCount];
    attr->attributeName = arenaAllocate(parseState->arena, sizeof(char) * parseState->currentIndex + 1);
    if (attr->attributeName == NULL)
    {
        printf("Error Code: %d\n", errno);
        perror("attributeName");
        exit(EXIT_FAILURE);
    }

    for (int j = 0; j < parseState->currentIndex; j++)
    {
        attr->attributeName[j] = parseState->currentItem[j];
    }

    attr->attributeName[parseState->currentIndex] = '\0';
    attr->attributeValue = NULL;
    tag->attributeCount++;
    parseState->currentIndex = 0;
    parseState->currentItem[0] = '\0';
    parseState->attributeNameAdded = true;
    parseState->nonWhiteSpaceInTextContent = false;

    return 0;
}

int handleNewTag(ParseState *parseState)
{
    size_t *tagLocation = &parseState->arena->size;
    HtmlTag *tag = arenaAllocate(parseState->arena, sizeof(HtmlTag));
    if (tag == NULL)
    {
        printf("Error Code: %d\n", errno);
        perror("htmlTags");
        exit(EXIT_FAILURE);
    }

    tag->children = NULL;
    tag->tagCount = 0;
    tag->maxSize = 0;

    HtmlTag *currentTag = peekCurrentTag(parseState);
    if (currentTag->tagCount == 0)
    {
        currentTag->children = arenaAllocate(parseState->arena, sizeof(HtmlTag *));
        currentTag->maxSize = sizeof(HtmlTag *);
        if (!currentTag->children)
        {
            printf("Error Code: %d\n", errno);
            perror("currentTag->children");
            exit(EXIT_FAILURE);
        }
    }
    else
    {
        size_t newSize = sizeof(HtmlTag *) * (currentTag->tagCount + 1);
        if (newSize > currentTag->maxSize)
        {
            newSize = sizeof(HtmlTag *) * (currentTag->tagCount * 2);
            HtmlTag **tmpTags = areanReallocate(parseState->arena, currentTag->children, currentTag->maxSize, newSize);
            if (!tmpTags)
            {
                printf("Error Code: %d\n", errno);
                perror("*tmpTags");
                exit(EXIT_FAILURE);
            }

            currentTag->children = tmpTags;
            currentTag->maxSize = newSize;
        }
    }

    printf("Tag Name: %s\n", currentTag->tagName);
    printf("Tag Count: %d\n", currentTag->tagCount);

    currentTag->children[currentTag->tagCount] = tag;
    currentTag->tagCount++;

    pushStack(parseState->htmlTags, tagLocation);
    tag->attributeCount = 0;
    tag->tagName = arenaAllocate(parseState->arena, parseState->currentIndex + 1);
    if (tag->tagName == NULL)
    {
        printf("Error Code: %d\n", errno);
        perror("tagName");
        exit(EXIT_FAILURE);
    }

    for (int j = 0; j < parseState->currentIndex; j++)
    {
        if (parseState->currentItem[j] == '\n' || parseState->currentItem[j] == ' ')
        {
            break;
        }

        tag->tagName[j] = parseState->currentItem[j];
    }

    tag->tagName[parseState->currentIndex] = '\0';
    parseState->currentIndex = 0;
    parseState->currentItem[0] = '\0';
    parseState->nonWhiteSpaceInTextContent = false;
    return 0;
}

int appendCharToItem(ParseState *parseState, char c)
{
    if (!parseState->inTag && !parseState->nonWhiteSpaceInTextContent && c != ' ' && c != '\n' && c != '\t' && c != '\r')
    {
        parseState->nonWhiteSpaceInTextContent = true;
    }

    size_t newSize = sizeof(char) * parseState->currentIndex;
    if (newSize >= parseState->maxSize)
    {
        newSize = parseState->maxSize * 2;
        void *tmp = areanReallocate(parseState->arena, parseState->currentItem, parseState->maxSize, newSize);
        if (!tmp)
        {
            printf("Error Code: %d\n", errno);
            perror("*tmp");
            exit(EXIT_FAILURE);
        }

        parseState->currentItem = tmp;
        parseState->maxSize = newSize;
    }

    parseState->currentItem[parseState->currentIndex++] = c;

    return 0;
}
