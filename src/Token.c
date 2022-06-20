#include "Token.h"

const char* TOKEN_TO_STRING[] = {
    #define TOKEN(t) [TOKEN_##t] = #t,
    TOKENS
    #undef TOKEN
};

Token createToken(TokenType type, const char* lexeme, size_t length, int line, int column) {
    return (Token) {
        .Type = type,
        .Lexeme = lexeme,
        .Length = length,
        .Line = line,
        .Column = column
    };
}