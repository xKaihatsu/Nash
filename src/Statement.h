#ifndef STATEMENT_H
#define STATEMENT_H

#include "Expression.h"
#include "Declaration.h"

typedef struct Statement Statement;
typedef enum StatementType {
    STATEMENT_UNKNOWN,
    STATEMENT_EXPRESSION,
    STATEMENT_DECLARATION,
    STATEMENT_BLOCK,
    STATEMENT_IF,
    STATEMENT_RETURN
} StatementType;

typedef struct StatementBlock {
    size_t Count;
    Statement** Statements;
} StatementBlock;

typedef struct IfStatement {
    Expression* Condition;
    Statement* Block;
    Statement* ElseBlock;
} IfStatement;

struct Statement {
    StatementType Type;
    union {
        Expression* Expresssion;
        Declaration* Declaration;
        StatementBlock* Block;
        IfStatement If;
    };
};

Statement* newExpressionStatement(Expression* expression);
Statement* newDeclarationStatement(Declaration* declaration);
Statement* newIfStatement(Expression* condition, Statement* block, Statement* elseBlock);
Statement* newReturnStatement(Expression* expression);

Statement* newStatementBlock();
void addStatement(StatementBlock* statementBlock, Statement* statement);
void addExpression(StatementBlock* statementBlock, Expression* expression);
void addDeclaration(StatementBlock* statementBlock, Declaration* declaration);


#endif