#include "parser/parser.h"
#include "parser/parser-structs.h"

int main(int argc, char *argv[])
{
    CURLcode result = curl_global_init(CURL_GLOBAL_ALL);
    if (result != CURLE_OK)
    {
        return -1;
    }

    HtmlDoc *html_doc = parse_html("https://www.example.com");
    // struct HtmlDoc *html_doc = parse_html("https://parkerstovall.com");
    if (html_doc == NULL)
    {
        printf("Error during html parse\n");
        return 1;
    }

    printf("Found %d tags!\n", html_doc->size);

    for (int i = 0; i < html_doc->size; i++)
    {
        if (!html_doc->htmlTags[i])
        {
            continue;
        }

        printf("Tag Found: %s, with %d attributes.\n", html_doc->htmlTags[i]->tagName, html_doc->htmlTags[i]->size);
        for (int j = 0; j < html_doc->htmlTags[i]->size; j++)
        {
            printf("\tAttribute Found: %s=\"%s\"\n", html_doc->htmlTags[i]->attributes[j]->attributeName, html_doc->htmlTags[i]->attributes[j]->attributeValue);
        }
    }

    free_html_doc(html_doc);

    /* we are done with libcurl, so clean it up */
    curl_global_cleanup();
}
