#include <curl/curl.h>
#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include "parser-structs.h"
#include "parser-utils.h"
#include "../utils/stack.h"

static bool tagNameNeedsClosingTag(const char *tagName)
{
    if (tagName == NULL)
    {
        return true;
    }

    static const char *voidTags[] = {
        "area", "base", "br", "col", "embed", "hr", "img",
        "input", "link", "meta", "param", "source", "track", "wbr"};
    size_t count = sizeof(voidTags) / sizeof(voidTags[0]);

    for (size_t i = 0; i < count; i++)
    {
        if (strcasecmp(voidTags[i], tagName) == 0)
        {
            return false;
        }
    }

    return true;
}

size_t parseResponse(char *contents, size_t size, size_t nmemb, void *userp)
{
    ParseState *parseState = (ParseState *)userp;

    size_t realsize = size * nmemb;

    for (size_t i = 0; i < realsize; i++)
    {
        char c = contents[i];

        if (c == '<')
        {
            parseState->inTag = true;
            parseState->justOpenedTag = true;
            continue;
        }
        else if (parseState->tagAdded && c == '=' && !parseState->attributeNameAdded)
        {
            int err = handleNewAttribute(parseState);
            if (err != 0)
            {
                return err;
            }
        }
        else if (parseState->attributeNameAdded)
        {
            if (c != '"')
            {
                int err = appendCharToItem(parseState, c);
                if (err != 0)
                {
                    return err;
                }
            }
            else if (parseState->currentIndex > 0)
            {
                int err = handleAttributeValue(parseState);
                if (err != 0)
                {
                    return err;
                }
            }
        }
        else if (parseState->inTag && (c == ' ' || c == '\n'))
        {
            if (parseState->attributeNameAdded)
            {
                int err = handleAttributeValue(parseState);
                if (err != 0)
                {
                    return err;
                }
            }
            else if (!parseState->tagAdded)
            {
                int err = handleNewTag(parseState);
                if (err != 0)
                {
                    return err;
                }

                parseState->tagAdded = true;
            }
        }
        else if (parseState->inTag && c == '>')
        {
            HtmlTag *currentTag = peekStack(parseState->htmlTags);
            if (!tagNameNeedsClosingTag(currentTag->tagName))
            {
                popStack(parseState->htmlTags, true);
            }

            if (parseState->currentIndex > 0)
            {
                if (!parseState->tagAdded)
                {
                    int err = handleNewTag(parseState);
                    if (err != 0)
                    {
                        return err;
                    }
                }
                else
                {
                    int err = handleNewAttribute(parseState);
                    if (err != 0)
                    {
                        return err;
                    }
                }
            }

            parseState->tagCount++;
            parseState->inTag = false;
            parseState->tagAdded = false;
        }
        // Skip closing tags
        else if (parseState->inTag && (c == '/' || c == '!'))
        {
            if (parseState->justOpenedTag)
            {
                parseState->inTag = false;
                parseState->tagAdded = false;
                if (parseState->htmlTags->size > 1) // This guard ignores '<!doctype>' headers
                {
                    popStack(parseState->htmlTags, true);
                }
            }
        }
        else if (parseState->inTag)
        {
            int err = appendCharToItem(parseState, c);
            if (err != 0)
            {
                return err;
            }
        }

        parseState->justOpenedTag = false;
    }

    return realsize;
}

ParseState *newParseState()
{
    ParseState *parseState = malloc(sizeof(ParseState));
    if (!parseState)
    {
        printf("Error Code: %d\n", errno);
        perror("parseState");
        return NULL;
    }

    HtmlTag *rootTag = malloc(sizeof(HtmlTag));
    rootTag->children = NULL;
    rootTag->attributeCount = 0;
    rootTag->tagCount = 0;
    rootTag->tagName = "ROOT";
    parseState->htmlTags = newStack(10, 10);
    pushStack(parseState->htmlTags, rootTag);

    parseState->currentIndex = 0;
    parseState->tagCount = 0;
    parseState->inTag = false;
    parseState->tagAdded = false;
    parseState->maxSize = 8 * sizeof(parseState->currentItem);
    parseState->currentItem = malloc(parseState->maxSize);

    return parseState;
}

HtmlTag *parseHtml(char *url)
{
    CURL *curl;

    ParseState *parseState = newParseState();
    if (parseState == NULL)
    {
        return NULL;
    }

    if (!parseState->currentItem)
    {
        printf("Error Code: %d\n", errno);
        perror("current_item");
        freeParseState(parseState, true);
        return NULL;
    }

    /* init the curl session */
    curl = curl_easy_init();
    if (!curl)
    {
        freeParseState(parseState, true);
        return NULL;
    }

    /* specify URL to get */
    curl_easy_setopt(curl, CURLOPT_URL, url);

    /* send all data to this function */
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, parseResponse);

    /* pass custom param to function */
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, parseState);

    /* some servers do not like requests that are made without a user-agent
       field, so we provide one */
    curl_easy_setopt(curl, CURLOPT_USERAGENT, "libcurl-agent/1.0");

    /* get it! */
    CURLcode result = curl_easy_perform(curl);

    /* cleanup curl stuff */
    curl_easy_cleanup(curl);

    /* check for errors */
    if (result != CURLE_OK)
    {
        fprintf(stderr, "curl_easy_perform() failed: %s\n",
                curl_easy_strerror(result));

        freeParseState(parseState, true);
        return NULL;
    }

    HtmlTag *htmlTag = popStack(parseState->htmlTags, false);
    freeParseState(parseState, false);
    return htmlTag;
}