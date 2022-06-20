#ifndef DECLARATION_H
#define DECLARATION_H

#include "Common.h"
#include "Expression.h"

typedef struct Declaration Declaration;
typedef struct Statement Statement;
typedef enum DeclarationType {
    DECLARATION_UNKNOWN,
    DECLARATION_VARIABLE,
    DECLARATION_FUNCTION
} DeclarationType;

typedef struct VariableDeclaration {
    Expression* Initializer;
} VariableDeclaration;

typedef struct FunctionDeclaration {
    Declaration** Parameters;
    size_t Arity;
    // TODO: add proper type system
    const char* ReturnType;
    Statement* Block;
} FunctionDeclaration;

struct Declaration {
    const char* Name;
    DeclarationType Type;
    union {
        VariableDeclaration Variable;
        FunctionDeclaration Function;
    };
};

Declaration* newVariableDeclaration(const char* name, Expression* initializer);
Declaration* newFunctionDeclaration(const char* name, Declaration** parameters, size_t arity, const char* returnType, Statement* block);

#endif