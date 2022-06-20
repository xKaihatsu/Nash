#ifndef LEXER_H
#define LEXER_H

#include "Common.h"
#include "Token.h"

typedef struct Lexer {
    char* Source;
    const char* CurrentCharacter;
    Token* Tokens;
    int Line, Column;
} Lexer;

Lexer* newLexer(const char* sourceFile);
void freeLexer(Lexer* lexer);

Token* scanTokens(Lexer* lexer);
void printTokens(Lexer* lexer);

#endif