#include "Expression.h"
#include <stdlib.h>

const char* OPERATION_TO_STRING[] = {
    #define OPERATION(op, s) [OPERATION_##op] = s,
    OPERATIONS
    #undef OPERATION
};

static Expression* newExpression(ExpressionType type) {
    Expression* expression = calloc(1, sizeof(Expression));
    expression->Type = type;
    return expression;
}

static Expression* newLiteral(LiteralType type) {
    Expression* literal = newExpression(EXPRESSION_LITERAL);
    literal->Literal.Type = type;
    return literal;
}

Expression* newIntegerLiteral(int value) {
    Expression* literal = newLiteral(LITERAL_INTEGER);
    literal->Literal.Integer = value;
    return literal;
}
Expression* newFloatLiteral(double value) {
    Expression* literal = newLiteral(LITERAL_FLOAT);
    literal->Literal.Float = value;
    return literal;
}
Expression* newStringLiteral(const char* value) {
    Expression* literal = newLiteral(LITERAL_STRING);
    literal->Literal.String = value;
    return literal;
}
Expression* newUnaryExpression(Operation operation, Expression* expression) {
    Expression* unary = newExpression(EXPRESSION_UNARY);
    unary->Unary.Operation = operation;
    unary->Unary.Expression = expression;
    return unary;
}
Expression* newBinaryExpression(Operation operation, Expression* left, Expression* right) {
    Expression* binary = newExpression(EXPRESSION_BINARY);
    binary->Binary.Operation = operation;
    binary->Binary.Left = left;
    binary->Binary.Right = right;
    return binary;
}
Expression* newFunctionCall(const char* name, Expression** arguments, size_t arity) {
    Expression* call = newExpression(EXPRESSION_CALL);
    call->Call.Name = name;
    call->Call.Arguments = arguments;
    call->Call.Arity = arity;
    return call;
}

Expression* newVariable(const char* name) {
    Expression* variable = newExpression(EXPRESSION_VARIABLE);
    variable->Variable = name;
    return variable;
}

/*
Operator Precedence
*, / -> 4
+, - -> 3
>, >=, <, <= -> 2
==, == -> 1
*/
int operatorPrecedence(Operation operation) {
    switch (operation) {
        case OPERATION_NOT_EQUAL_TO:
        case OPERATION_EQUAL_TO: return 1;
        case OPERATION_LESS_THAN:
        case OPERATION_LESS_THAN_OR_EQUAL:
        case OPERATION_GREATER_THAN:
        case OPERATION_GREATER_THAN_OR_EQUAL: return 2;
        case OPERATION_ADD:
        case OPERATION_SUBTRACT: return 3;
        case OPERATION_MULTIPLY:
        case OPERATION_DIVIDE: return 4;
    }

    return 0;
}