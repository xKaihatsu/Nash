#ifndef EXPRESSION_H
#define EXPRESSION_H

#include "Common.h"

typedef struct Expression Expression;

#define OPERATIONS \
        OPERATION(ADD, "+") \
        OPERATION(SUBTRACT, "-") \
        OPERATION(MULTIPLY, "*") \
        OPERATION(DIVIDE, "/") \
        OPERATION(GREATER_THAN, ">") \
        OPERATION(GREATER_THAN_OR_EQUAL, ">=") \
        OPERATION(LESS_THAN, "<") \
        OPERATION(LESS_THAN_OR_EQUAL, "<=") \
        OPERATION(EQUAL_TO, "==") \
        OPERATION(NOT_EQUAL_TO, "!=") \
        OPERATION(UNKNOWN, "???") \
// > >= < <= == !=

typedef enum Operation {
    #define OPERATION(op, _) OPERATION_##op,
    OPERATIONS
    #undef OPERATION
} Operation;
extern const char* OPERATION_TO_STRING[];

typedef enum ExpressionType {
    EXPRESSION_UNKNOWN,
    EXPRESSION_LITERAL,
    EXPRESSION_UNARY,
    EXPRESSION_BINARY,
    EXPRESSION_CALL,
    EXPRESSION_VARIABLE
} ExpressionType;

typedef enum LiteralType {
    LITERAL_UNKNOWN,
    LITERAL_INTEGER,
    LITERAL_FLOAT,
    LITERAL_STRING
} LiteralType;

typedef struct Literal {
    LiteralType Type;
    union {
        uint64_t Integer;
        double Float;
        const char* String;
    };
} Literal;

typedef struct UnaryExpression {
    Operation Operation;
    Expression* Expression;
} UnaryExpression;

typedef struct FunctionCall {
    const char* Name;
    size_t Arity;
    Expression** Arguments;
} FunctionCall;

typedef struct BinaryExpression {
    Operation Operation;
    Expression* Left;
    Expression* Right;
} BinaryExpression;

struct Expression {
    ExpressionType Type;
    union {
        Literal Literal;
        UnaryExpression Unary;
        BinaryExpression Binary;
        FunctionCall Call;
        const char* Variable;
    };
};

Expression* newIntegerLiteral(int value);
Expression* newFloatLiteral(double value);
Expression* newStringLiteral(const char* value);
Expression* newUnaryExpression(Operation operation, Expression* expression);
Expression* newBinaryExpression(Operation operation, Expression* left, Expression* right);
Expression* newFunctionCall(const char* name, Expression** arguments, size_t arity);
Expression* newVariable(const char* name);

int operatorPrecedence(Operation operation);

#endif