#ifndef TOKEN_H
#define TOKEN_H

#include "Common.h"

#define TOKENS \
        TOKEN(UNKNOWN) \
        TOKEN(PUNCTUATOR) \
        TOKEN(KEYWORD) \
        TOKEN(IDENTIFIER) \
        TOKEN(INTEGER) \
        TOKEN(REAL) \
        TOKEN(STRING) \
        TOKEN(EOF) \

typedef enum TokenType {
    #define TOKEN(t) TOKEN_##t,
    TOKENS
    #undef TOKEN
} TokenType;

typedef struct Token {
    TokenType Type;
    const char* Lexeme;
    size_t Length;
    int Line, Column;
    union {
        uint64_t Integer;
        double Real;
        const char* String;
        char* Name;
    };
} Token;

extern const char* TOKEN_TO_STRING[];

Token createToken(TokenType type, const char* lexeme, size_t length, int line, int column);

#endif