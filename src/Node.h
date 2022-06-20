#ifndef NODE_H
#define NODE_H

#include "Expression.h"
#include "Declaration.h"
#include "Statement.h"
#include <stdio.h>

typedef struct Node Node;
typedef enum NodeType {
    NODE_UNKNOWN,
    NODE_EXPRESSION,
    NODE_DECLARATION,
    NODE_STATEMENT,
    NODE_PROGRAM
} NodeType;

typedef struct ProgramNode {
    size_t Count;
    Node** Nodes;
} ProgramNode;

struct Node {
    NodeType Type;
    // Token Token?
    union {
        Expression* Expression;
        Declaration* Declaration;
        Statement* Statement;
        ProgramNode* Program;
    };
};


Node* newExpressionNode(Expression* expression);
Node* newDeclarationNode(Declaration* expression);
Node* newStatementNode(Statement* expression);
Node* newProgramNode();
void addNode(ProgramNode* program, Node* node);

void dumpExpression(FILE* stream, Expression* expression, unsigned int indentation);
void dumpDeclaration(FILE* stream, Declaration* declaration, unsigned int indentation);
void dumpStatement(FILE* stream, Statement* statement, unsigned int indentation);
void dumpNode(FILE* stream, Node* node);

#endif