#ifndef PARSER_H
#define PARSER_H

#include "Common.h"
#include "Token.h"
#include "Node.h"

typedef struct Parser {
    Token* Tokens;
    Token* CurrentToken;
} Parser;

Parser* newParser(Token* tokens);
void freeParser(Parser* parser);

Node* parse(Parser* parser);

#endif