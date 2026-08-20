#include <curl/curl.h>
#include <errno.h>
#include <stdlib.h>
#include "parser-structs.h"
#include "parser-utils.h"

size_t parse_response(char *contents, size_t size, size_t nmemb, void *userp)
{
    ParseState *parse_state = (ParseState *)userp;

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

ParseState *get_parse_state()
{
    ParseState *parse_state = malloc(sizeof(ParseState));
    if (!parse_state)
    {
        printf("Error Code: %d\n", errno);
        perror("parse_state");
        return NULL;
    }

    parse_state->html_doc = malloc(sizeof(HtmlDoc));
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

HtmlDoc *parse_html(char *url)
{
    CURL *curl;

    ParseState *parse_state = get_parse_state();
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

    HtmlDoc *html_doc = parse_state->html_doc;
    free_parse_state(parse_state, false);
    return html_doc;
}