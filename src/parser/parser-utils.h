#ifndef PARSER_UTILS_C_ /* Include guard */
#define PARSER_UTILS_C_

#include "parser-structs.h"

void free_html_doc(HtmlDoc *html_doc);

void free_parse_state(ParseState *parse_state, bool freeDoc);

int handle_attribute_value(ParseState *parse_state);

int handle_attribute(ParseState *parse_state);

int handle_item_break(ParseState *parse_state);

int append_char_to_item(ParseState *parse_state, char c);

#endif // PARSER_UTILS_C_
