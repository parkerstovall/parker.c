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

void free_html_doc(struct HtmlDoc *html_doc)
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

void free_parse_state(struct ParseState *parse_state, bool freeDoc)
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

static void print_current_item(struct ParseState *parse_state)
{
    for (int i = 0; i < parse_state->current_index; i++)
    {
        printf("%c", parse_state->current_item[i]);
    }

    printf("\n");
}

static int handle_attribute_value(struct ParseState *parse_state)
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

static int handle_attribute(struct ParseState *parse_state)
{
    // NEW WAY WITH STRUCT
    size_t new_tag_size = sizeof(struct HtmlTag) + ((parse_state->attribute_count + 1) * sizeof(struct HtmlAttribute *));
    struct HtmlTag *tmpTag = realloc(parse_state->html_doc->htmlTags[parse_state->tag_count], new_tag_size);
    if (!tmpTag)
    {
        printf("Error Code: %d\n", errno);
        perror("*tmpTag");
        return errno;
    }

    parse_state->html_doc->htmlTags[parse_state->tag_count] = tmpTag;
    parse_state->html_doc->htmlTags[parse_state->tag_count]->attributes[parse_state->attribute_count] = malloc(sizeof(struct HtmlAttribute));
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

static int handle_item_break(struct ParseState *parse_state)
{
    // NEW WAY WITH STRUCT
    size_t new_doc_size = sizeof(struct HtmlDoc) + ((parse_state->tag_count + 1) * sizeof(struct HtmlTag *));
    struct HtmlDoc *tmpDoc = realloc(parse_state->html_doc, new_doc_size);
    if (tmpDoc == NULL)
    {
        printf("Error Code: %d\n", errno);
        perror("*tmpDoc");
        return errno;
    }

    parse_state->html_doc = tmpDoc;
    parse_state->html_doc->size++;

    parse_state->html_doc->htmlTags[parse_state->tag_count] = malloc(sizeof(struct HtmlTag));
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

static int append_char_to_item(struct ParseState *parse_state, char c)
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

static size_t parse_response(char *contents, size_t size, size_t nmemb, void *userp)
{
    struct ParseState *parse_state = (struct ParseState *)userp;

    size_t realsize = size * nmemb;

    for (size_t i = 0; i < realsize; i++)
    {
        char c = contents[i];

        if (c == '<')
        {
            parse_state->in_tag = true;
            parse_state->just_opened_tag = true;
            continue;
        }
        else if (parse_state->tag_added && c == '=' && !parse_state->attribute_name_added)
        {
            int err = handle_attribute(parse_state);
            if (err != 0)
            {
                return err;
            }
        }
        else if (parse_state->attribute_name_added)
        {
            if (c != '"')
            {
                int err = append_char_to_item(parse_state, c);
                if (err != 0)
                {
                    return err;
                }
            }
            else if (parse_state->current_index > 0)
            {
                int err = handle_attribute_value(parse_state);
                if (err != 0)
                {
                    return err;
                }
            }
        }
        else if (parse_state->in_tag && (c == ' ' || c == '\n'))
        {
            if (parse_state->attribute_name_added)
            {
                int err = handle_attribute_value(parse_state);
                if (err != 0)
                {
                    return err;
                }
            }
            else if (!parse_state->tag_added)
            {
                int err = handle_item_break(parse_state);
                if (err != 0)
                {
                    return err;
                }

                parse_state->tag_added = true;
            }
        }
        else if (parse_state->in_tag && c == '>')
        {
            if (parse_state->current_index > 0)
            {
                if (!parse_state->tag_added)
                {
                    int err = handle_item_break(parse_state);
                    if (err != 0)
                    {
                        return err;
                    }
                }
                else
                {
                    int err = handle_attribute(parse_state);
                    if (err != 0)
                    {
                        return err;
                    }
                }
            }

            parse_state->tag_count++;
            parse_state->in_tag = false;
            parse_state->tag_added = false;
        }
        // Skip closing tags
        else if (parse_state->in_tag && (c == '/' || c == '!'))
        {
            if (parse_state->just_opened_tag)
            {
                parse_state->in_tag = false;
                parse_state->tag_added = false;
            }
        }
        else if (parse_state->in_tag)
        {
            int err = append_char_to_item(parse_state, c);
            if (err != 0)
            {
                return err;
            }
        }

        parse_state->just_opened_tag = false;
    }

    return realsize;
}

struct ParseState *get_parse_state()
{
    struct ParseState *parse_state = malloc(sizeof(struct ParseState));
    if (!parse_state)
    {
        printf("Error Code: %d\n", errno);
        perror("parse_state");
        return NULL;
    }

    parse_state->html_doc = malloc(sizeof(struct HtmlDoc));
    if (!parse_state->html_doc)
    {
        printf("Error Code: %d\n", errno);
        perror("html_doc");
        free_parse_state(parse_state, false);
        return NULL;
    }

    parse_state->html_doc->size = 0;
    parse_state->current_index = 0;
    parse_state->tag_count = 0;
    parse_state->in_tag = false;
    parse_state->tag_added = false;
    parse_state->max_size = 8 * sizeof(parse_state->current_item);
    parse_state->current_item = malloc(parse_state->max_size);

    return parse_state;
}

struct HtmlDoc *parse_html(char *url)
{
    CURL *curl;

    struct ParseState *parse_state = get_parse_state();
    if (parse_state == NULL)
    {
        return NULL;
    }

    if (!parse_state->current_item)
    {
        printf("Error Code: %d\n", errno);
        perror("current_item");
        free_parse_state(parse_state, true);
        return NULL;
    }

    /* init the curl session */
    curl = curl_easy_init();
    if (!curl)
    {
        free_parse_state(parse_state, true);
        return NULL;
    }

    /* specify URL to get */
    curl_easy_setopt(curl, CURLOPT_URL, url);

    /* send all data to this function */
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, parse_response);

    /* pass custom param to function */
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, parse_state);

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

        free_parse_state(parse_state, true);
        return NULL;
    }

    struct HtmlDoc *html_doc = parse_state->html_doc;
    free_parse_state(parse_state, false);
    return html_doc;
}

int main(int argc, char *argv[])
{
    CURLcode result = curl_global_init(CURL_GLOBAL_ALL);
    if (result != CURLE_OK)
    {
        return -1;
    }

    struct HtmlDoc *html_doc = parse_html("https://www.example.com");
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
