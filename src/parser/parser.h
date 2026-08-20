#ifndef PARSER_C_ /* Include guard */
#define PARSER_C_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <curl/curl.h>
#include <stdbool.h>
#include <errno.h>
#include "parser-structs.h"

void free_parse_state(ParseState *parse_state, bool freeDoc);

void free_html_doc(HtmlDoc *html_doc);

ParseState *get_parse_state();

HtmlDoc *parse_html(char *url);

size_t parse_response(char *contents, size_t size, size_t nmemb, void *userp);

#endif // PARSER_C_
