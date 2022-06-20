#include "Lexer.h"
#include "Common.h"
#include "Token.h"
#include "StretchyBuffer.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>

Lexer* newLexer(const char* sourceFile) {
    Lexer* lexer = calloc(1, sizeof(Lexer));

    size_t sourceLength = strlen(sourceFile);
    lexer->Source = strncpy(calloc(sourceLength + 1, sizeof(char)), sourceFile, sourceLength);
    lexer->CurrentCharacter = lexer->Source;
    lexer->Line = 1;
    lexer->Column = 1;
    lexer->Tokens = newStretchyBuffer(sizeof(Token));
    return lexer;
}

void freeLexer(Lexer* lexer) {
    free(lexer->Source);
    freeStretchyBuffer(lexer->Tokens);
}

/*
    Advance one character.
*/
static char advance(Lexer* lexer) {
    // TODO: consider adding proper bounds checking
    char currentCharacter = *++lexer->CurrentCharacter;
    
    if (currentCharacter == '\n') {
        lexer->Line++;
        lexer->Column = 1;
    } else {
        lexer->Column++;
    }
    
    return currentCharacter;
}

/*
    Advance a given amount of steps and return the current character.
*/
static char step(Lexer* lexer, unsigned int steps) {
    while (steps-- > 0) {
        advance(lexer);
    }
    return *lexer->CurrentCharacter;
}

static size_t getLexerIndex(Lexer* lexer) {
    return lexer->CurrentCharacter - lexer->Source;
}

/*
    Peek one character ahead.
*/
static char peek(Lexer* lexer) {
    return *(lexer->CurrentCharacter + 1);
}

/*
    Returns true if `lexeme` matches the lexer's current lexeme.
    Returns false otherwise.
*/
static bool consume(Lexer* lexer, const char* lexeme) {
    size_t lexemeLength = strlen(lexeme);
    if (strneq(lexer->CurrentCharacter, lexeme, lexemeLength)) {
        step(lexer, lexemeLength);
        return true;
    }

    return false;
}

static bool isIdentifier(char c) {
    return isalnum(c) || c == '_';
}

/*
    Returns the length of the punctuator if it's a valid lexeme.
    Returns 0 if the input lexeme is not a valid punctuator.
*/
static size_t isPunctuator(const char* lexeme) {
    static const char* punctuators[] = {
        "->", "++", "--", ">>", "<<", "<=", ">=", "==", "!=", "&&",
        "||", "*=", "/=", "%=", "+=", "-=", "<<=", ">>=", "&=", "^=",
        "|=", "..."
    };
    static const size_t punctuatorCount = sizeof(punctuators) / sizeof(*punctuators);
    static const char** punctuatorsEnd = punctuators + punctuatorCount;

    for (const char** punctuator = punctuators; punctuator != punctuatorsEnd; punctuator++) {
        size_t punctuatorLength = strlen(*punctuator);
        if (strneq(lexeme, *punctuator, punctuatorLength)) {
            return punctuatorLength;
        }
    }

    return ispunct(*lexeme) ? 1 : 0;
}

static bool isKeyword(const char* lexeme, size_t lexemeLength) {
    static const char* keywords[] = {
        "any", "as", "bool", "break", "case", "char", "class", "const", "continue", "default", "do", "double", "else",
        "enum", "false", "float", "for", "function", "goto", "if", "import", "inline", "int", "let", "namespace",
        "null", "private", "protected", "public", "return", "short", "signed", "sizeof", "static", "string", "struct",
        "switch", "this", "true", "typeof", "union", "unsigned", "void", "while"
    };
    static const size_t keywordCount = sizeof(keywords) / sizeof(*keywords);
    static const char** keywordsEnd = keywords + keywordCount;

    for (const char** keyword = keywords; keyword != keywordsEnd; keyword++) {
        size_t keywordLength = strlen(*keyword);
        if (strneq(lexeme, *keyword, keywordLength)) {
            return true;
        }
    }

    return false;
}

Token* scanTokens(Lexer* lexer) {
    if (bufferLength(lexer->Tokens) > 0) {
        return lexer->Tokens;
    }

    char currentCharacter = *lexer->CurrentCharacter;
    while (currentCharacter) {
        currentCharacter = *lexer->CurrentCharacter;
        if (isspace(currentCharacter)) {
            do {
                currentCharacter = advance(lexer);
            } while (isspace(currentCharacter));
            continue;
        }

        size_t start = getLexerIndex(lexer);
        Token token = createToken(TOKEN_UNKNOWN, lexer->CurrentCharacter, 1, lexer->Line, lexer->Column);
        
        if (isalpha(currentCharacter)) {
            token.Type = TOKEN_IDENTIFIER;
            do {
                currentCharacter = advance(lexer);
            } while (isIdentifier(currentCharacter));
            token.Length = getLexerIndex(lexer) - start;
            token.Name = strndup(token.Lexeme, token.Length);

            if (isKeyword(token.Lexeme, token.Length)) {
                token.Type = TOKEN_KEYWORD;
            }
        } else if (isdigit(currentCharacter) || (currentCharacter == '.' && isdigit(peek(lexer)))) {
            // TODO: convert integer to an actual number
            token.Type = TOKEN_INTEGER;
            int base = 10;
            double value = 0;

            while (isdigit(currentCharacter)) {
                int digit = currentCharacter - '0';
                value = digit + value * base;
                currentCharacter = advance(lexer);
            }

            if (consume(lexer, ".")) {
                token.Type = TOKEN_REAL;
                do {
                    currentCharacter = advance(lexer);
                } while (isdigit(currentCharacter));
            }
            
            if (consume(lexer, "e") || consume(lexer, "E")) {
                token.Type = TOKEN_REAL;
                consume(lexer, "-") || consume(lexer, "+");
                do {
                    currentCharacter = advance(lexer);
                } while (isdigit(currentCharacter));
            }

            if (token.Type == TOKEN_INTEGER) {
                token.Integer = (int)value;
            } else {
                token.Real = value;
            }

            token.Length = getLexerIndex(lexer) - start;
        } else if (consume(lexer, "\"")) {
            token.Type = TOKEN_STRING;
            do {
                currentCharacter = advance(lexer);
            } while (!consume(lexer, "\"") && currentCharacter);
            // TODO: make sure the string was closed correctly

            token.Length = getLexerIndex(lexer) - start;
        } else if (ispunct(currentCharacter)) {
            token.Type = TOKEN_PUNCTUATOR;
            token.Length = isPunctuator(token.Lexeme);
            step(lexer, token.Length);
        } else {
            if (currentCharacter == '\0') break;
            advance(lexer);
        }

        bufferPush(lexer->Tokens, token);
    }

    Token eof = createToken(TOKEN_EOF, lexer->CurrentCharacter, 1, lexer->Line, lexer->Column);
    bufferPush(lexer->Tokens, eof);
    return lexer->Tokens;
}

void printTokens(Lexer* lexer) {
    Token* tokens = scanTokens(lexer);
    for (Token* token = tokens; token != bufferEnd(tokens); token++) {
        printf("%s%s", TOKEN_TO_STRING[token->Type], token->Type != TOKEN_EOF ? ", " : "\0");
        if (token->Type != TOKEN_EOF) {
            printf("`%.*s`", (int)token->Length, token->Lexeme);
        }
        printf("\n");
    }
}