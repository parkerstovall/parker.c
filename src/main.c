#include "parser/parser.h"
#include "parser/parser-utils.h"
#include "parser/parser-structs.h"
#include "arena/arena.h"
#include <time.h>

static void printTags(HtmlTag *tag, int depth)
{
    for (int i = 0; i < depth; i++)
    {
        printf(" ");
    }

    printf("<%s", tag->tagName);
    for (int i = 0; i < tag->attributeCount; i++)
    {
        printf(" %s", tag->attributes[i]->attributeName);

        if (tag->attributes[i]->attributeValue)
        {
            printf("=\"%s\"", tag->attributes[i]->attributeValue);
        }
    }

    printf(">\n");

    for (int i = 0; i < tag->tagCount; i++)
    {
        printTags(tag->children[i], depth + 1);
    }

    for (int i = 0; i < depth; i++)
    {
        printf(" ");
    }

    printf("</%s>\n", tag->tagName);
}

static char *html = "<!doctype html><html lang=\"en\"><head><title>Example Domain</title><link rel=\"icon\" href=\"data:,\"><meta name=\"viewport\" content=\"width=device-width, initial-scale=1\"><style>CSS</style></head><body><div><h1>Example Domain</h1><p>This domain is for use in documentation examples without needing permission. Avoid use in operations.</p><p><a href=\"https://iana.org/domains/example\">Learn more</a></p></div></body></html>";

int main(int argc, char *argv[])
{
    ParseState *parseState = newParseState();
    parseResponse(html, strlen(html), 1, parseState);
    HtmlTag *htmlTag = popStack(parseState->htmlTags, false);
    printTags(htmlTag, 0);
    //  CURLcode result = curl_global_init(CURL_GLOBAL_ALL);
    //  if (result != CURLE_OK)
    //  {
    //      return -1;
    //  }

    // HtmlTag *htmlTag = parseHtml("https://www.example.com");
    // if (htmlTag == NULL)
    // {
    //     printf("Error during html parse\n");
    //     return 1;
    // }

    // printTags(htmlTag, 0);

    // freeHtmlTag(htmlTag);

    // /* we are done with libcurl, so clean it up */
    // curl_global_cleanup();
}
