#include "Declaration.h"
#include "Common.h"
#include "Expression.h"
#include "Statement.h"
#include <stdlib.h>

static Declaration* newDeclaration(const char* name, DeclarationType type) {
    Declaration* declaration = calloc(1, sizeof(Declaration));
    declaration->Name = name;
    declaration->Type = type;
    return declaration;
}

Declaration* newVariableDeclaration(const char* name, Expression* initializer) {
    Declaration* variableDeclaration = newDeclaration(name, DECLARATION_VARIABLE);
    variableDeclaration->Variable.Initializer = initializer;
    return variableDeclaration;
}

Declaration* newFunctionDeclaration(const char* name, Declaration** parameters, size_t arity, const char* returnType, Statement* block) {
    Declaration* functionDeclaration = newDeclaration(name, DECLARATION_FUNCTION);
    functionDeclaration->Function.Parameters = parameters;
    functionDeclaration->Function.Arity = arity;
    functionDeclaration->Function.ReturnType = returnType;
    functionDeclaration->Function.Block = block;
    return functionDeclaration;
}
