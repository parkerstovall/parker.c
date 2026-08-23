#include "parser/parser.h"
#include "parser/parser-utils.h"
#include "parser/parser-structs.h"

static void printTags(HtmlTag *tag, int depth)
{
    for (int i = 0; i < depth; i++)
    {
        printf("\t");
    }

    printf("<%s>\n", tag->tagName);
    for (int i = 0; i < tag->attributeCount; i++)
    {
        printTags(tag->children[i], depth + 1);
    }

    for (int i = 0; i < depth; i++)
    {
        printf("\t");
    }

    printf("</%s>\n", tag->tagName);
}

static char more_complicated_html[] = "<html>Testing<a href=\"https://www.example.com\">Link! <i attr1=\"test1\" attr2=\"test2\">with italics</i></a></html>";
int main(int argc, char *argv[])
{
    char expected[] = "i";
    ParseState *parseState = newParseState();
    parseResponse(more_complicated_html, sizeof(more_complicated_html), 1, parseState);
    HtmlTag *tag = popStack(parseState->htmlTags, false);
    HtmlTag *itag = tag->children[0]->children[0]->children[0];
    printf("%s\n", itag->tagName);

    // CURLcode result = curl_global_init(CURL_GLOBAL_ALL);
    // if (result != CURLE_OK)
    // {
    //     return -1;
    // }

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
