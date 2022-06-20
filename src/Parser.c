#include "Parser.h"
#include "Common.h"
#include "StretchyBuffer.h"
#include "Token.h"
#include "Node.h"
#include <stdlib.h>
#include <string.h>

static Expression* parseExpression(Parser* parser);
static Expression** parseExpressionList(Parser* parser);
static Statement* parseStatement(Parser* parser);

Parser* newParser(Token* tokens) {
    Parser* parser = calloc(1, sizeof(Parser));
    parser->Tokens = tokens;
    parser->CurrentToken = tokens;
    return parser;
}

void freeParser(Parser* parser) {
    free(parser);
}

static Token scanToken(Parser* parser) {
    return *parser->CurrentToken++;
}

static Token peek(Parser* parser) {
    Token peeked = *(parser->CurrentToken + 1);
    return peeked;
}

static bool consume(Parser* parser, const char* lexeme) {
    size_t lexemeLength = strlen(lexeme);
    if (parser->CurrentToken->Length == lexemeLength && strneq(parser->CurrentToken->Lexeme, lexeme, lexemeLength)) {
        scanToken(parser);
        return true;
    }
    return false;
}

static bool consumePunctuator(Parser* parser, const char* punctuator) {
    return parser->CurrentToken->Type == TOKEN_PUNCTUATOR && consume(parser, punctuator);
}

static bool consumeKeyword(Parser* parser, const char* keyword) {
    return parser->CurrentToken->Type == TOKEN_KEYWORD && consume(parser, keyword);
}

static Token expectIdentifier(Parser* parser) {
    Token token = *parser->CurrentToken;
    if (token.Type == TOKEN_IDENTIFIER) {
        scanToken(parser);
    }

    return token;
}

static Token expectKeyword(Parser* parser, const char* keyword) {
    Token token = *parser->CurrentToken;
    if (token.Type == TOKEN_KEYWORD && strneq(token.Lexeme, keyword, token.Length)) {
        scanToken(parser);
    }

    return token;
}

static Token expectPunctuator(Parser* parser, const char* punctuator) {
    Token token = *parser->CurrentToken;
    if (token.Type == TOKEN_PUNCTUATOR && strneq(token.Lexeme, punctuator, token.Length)) {
        scanToken(parser);
    }

    return token;
}

static bool matchKeyword(Parser* parser, const char* keyword) {
    return parser->CurrentToken->Type == TOKEN_KEYWORD && strneq(parser->CurrentToken->Lexeme, keyword, parser->CurrentToken->Length);
}

static bool matchPunctuator(Parser* parser, const char* punctuator) {
    return parser->CurrentToken->Type == TOKEN_PUNCTUATOR && strneq(parser->CurrentToken->Lexeme, punctuator, parser->CurrentToken->Length);
}

static Operation tokenToOperation(Token token) {
    for (Operation operation = 0; operation < OPERATION_UNKNOWN; operation++) {
        if (strneq(token.Lexeme, OPERATION_TO_STRING[operation], token.Length)) {
            return operation;
        }
    }

    return OPERATION_UNKNOWN;
}

static Expression* parseFunctionCall(Parser* parser) {

    Token functionName = expectIdentifier(parser);
    Token functionStart = expectPunctuator(parser, "(");
    Expression** arguments = parseExpressionList(parser);
    Token functionEnd = expectPunctuator(parser, ")");
    
    size_t arity = bufferLength(arguments);

    Expression* call = newFunctionCall(functionName.Name, arguments, arity);
    return call;
}

static Expression* parsePrimary(Parser* parser) {
    Token token = *parser->CurrentToken;
    switch (token.Type) {
        case TOKEN_INTEGER: {
            int value = token.Integer;
            scanToken(parser);
            return newIntegerLiteral(value);
        } break;
        case TOKEN_IDENTIFIER: {
            Token peeked = peek(parser);
            if (strneq(peeked.Lexeme, "(", peeked.Length)) {
                return parseFunctionCall(parser);
            }

            scanToken(parser);
            return newVariable(token.Name);
        } break;
        case TOKEN_PUNCTUATOR: {
            if (consume(parser, "(")) {
                Expression* expression = parseExpression(parser);
                consume(parser, ")");
                return expression;
            }
        }
    }

    return NULL;
}

static Expression* parseBinaryExpression(Parser* parser, int previousPrecedence) {
    Expression* left = NULL;
    
    left = parsePrimary(parser);
    Operation operation = tokenToOperation(*parser->CurrentToken);
    while (operatorPrecedence(operation) > previousPrecedence) {
        scanToken(parser);
        Expression* right = parseBinaryExpression(parser, operatorPrecedence(operation));
        left = newBinaryExpression(operation, left, right);
        operation = tokenToOperation(*parser->CurrentToken);
    }

    return left;
}

static Expression* parseExpression(Parser* parser) {
    return parseBinaryExpression(parser, 0);
}

static Expression** parseExpressionList(Parser* parser) {
    Expression** expressionList = newStretchyBuffer(sizeof(Expression*));

    Expression* expression = parseExpression(parser);
    if (expression) {
        bufferPush(expressionList, expression);
        while (consumePunctuator(parser, ",")) {
            expression = parseExpression(parser);
            bufferPush(expressionList, expression);
        }
    }

    return expressionList;
}

static Statement* parseBlock(Parser* parser) {
    Statement* block = newStatementBlock();

    Token blockStart = expectPunctuator(parser, "{");
    while (!matchPunctuator(parser, "}")) {
        Statement* statement = parseStatement(parser);
        addStatement(block->Block, statement);
    }
    Token blockEnd = expectPunctuator(parser, "}");
    
    return block;
}

static Declaration* parseVariableDeclaration(Parser* parser) {

    Token qualifier = expectKeyword(parser, "let");
    Token variableName = expectIdentifier(parser);
    Token variableType;
    if (consumePunctuator(parser, ":")) {
        variableType = scanToken(parser);
    }
    Expression* initializer = NULL;
    if (consumePunctuator(parser, "=")) {
        initializer = parseExpression(parser);
    }
    Token declarationEnd = expectPunctuator(parser, ";");

    Declaration* variableDeclaration = newVariableDeclaration(variableName.Name, initializer);
    return variableDeclaration;
}

static Declaration** parseFunctionParameters(Parser* parser) {
    Declaration** parameters = newStretchyBuffer(sizeof(Declaration*));
    
    // TODO: proper validation

    Declaration* parameter = NULL;
    if (parser->CurrentToken->Type == TOKEN_IDENTIFIER) {
        Token parameterName = expectIdentifier(parser);
        expectPunctuator(parser, ":");
        Token parameterType = scanToken(parser);

        parameter = newVariableDeclaration(parameterName.Name, NULL);
        bufferPush(parameters, parameter);
    }

    return parameters;
}

static Declaration* parseFunctionDeclaration(Parser* parser) {

    Token functionKeyword = expectKeyword(parser, "function");
    Token functionName = expectIdentifier(parser);
    expectPunctuator(parser, "(");
    Declaration** parameters = parseFunctionParameters(parser);
    size_t arity = bufferLength(parameters);
    expectPunctuator(parser, ")");
    if (consumePunctuator(parser, ":")) {
        Token functionReturnType = scanToken(parser);
    }
    
    Statement* block = parseBlock(parser);

    Declaration* functionDeclaration = newFunctionDeclaration(functionName.Name, parameters, arity, "void", block);
    return functionDeclaration;
}

static Statement* parseIfStatement(Parser* parser) {
    expectKeyword(parser, "if");
    expectPunctuator(parser, "(");
    Expression* condition = parseExpression(parser);
    expectPunctuator(parser, ")");
    Statement* block = parseBlock(parser);
    Statement* elseBlock = NULL;

    if (consumeKeyword(parser, "else")) {
        elseBlock = parseBlock(parser);
    }

    Statement* ifStatement = newIfStatement(condition, block, elseBlock);
    return ifStatement;
}

static Statement* parseReturnStatement(Parser* parser) {
    expectKeyword(parser, "return");
    Expression* expression = parseExpression(parser);
    expectPunctuator(parser, ";");
    Statement* returnStatement = newReturnStatement(expression);
    return returnStatement;
}

static Statement* parseStatement(Parser* parser) {
    Statement* statement = NULL;
    Token token = *parser->CurrentToken;

    switch (token.Type) {
        case TOKEN_KEYWORD: {
            if (matchKeyword(parser, "function")) {
                statement = newDeclarationStatement(parseFunctionDeclaration(parser));
            } else if (matchKeyword(parser, "let")) {
                statement = newDeclarationStatement(parseVariableDeclaration(parser));
            } else if (matchKeyword(parser, "if")) {
                statement = parseIfStatement(parser);
            } else if (matchKeyword(parser, "return")) {
                statement = parseReturnStatement(parser);
            }
        } break;
        default: {
            statement = newExpressionStatement(parseExpression(parser));
            expectPunctuator(parser, ";");
        }
    }

    return statement;
}

Node* parse(Parser* parser) {
    Node* program = newProgramNode();
    Token token = *parser->CurrentToken;
    while (token.Type != TOKEN_EOF) {
        Statement* statement = parseStatement(parser);
        addNode(program->Program, newStatementNode(statement));
        token = *parser->CurrentToken;
    }

    return program;
}