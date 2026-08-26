#ifndef PARSER_UTILS_C_ /* Include guard */
#define PARSER_UTILS_C_

#include "parser-structs.h"

void freeHtmlTag(HtmlTag *htmlTag);

void freeParseState(ParseState *parseState, bool freeTag);

int handleTextNode(ParseState *parseState);

int handleAttributeValue(ParseState *parseState);

int handleNewAttribute(ParseState *parseState);

int handleNewTag(ParseState *parseState);

int appendCharToItem(ParseState *parseState, char c);

#endif // PARSER_UTILS_C_
