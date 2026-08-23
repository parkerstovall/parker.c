#ifndef PARSER_C_ /* Include guard */
#define PARSER_C_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <curl/curl.h>
#include <stdbool.h>
#include <errno.h>
#include "parser-structs.h"

ParseState *newParseState();

size_t parseResponse(char *contents, size_t size, size_t nmemb, void *userp);

HtmlTag *parseHtml(char *url);

#endif // PARSER_C_
