#include "Statement.h"
#include "Expression.h"
#include "Statement.h"
#include "StretchyBuffer.h"
#include <stdlib.h>

static Statement* newStatement(StatementType type) {
    Statement* statement = calloc(1, sizeof(Statement));
    statement->Type = type;
    return statement;
}

Statement* newExpressionStatement(Expression* expression) {
    Statement* expressionStatement = newStatement(STATEMENT_EXPRESSION);
    expressionStatement->Expresssion = expression;
    return expressionStatement;
}
Statement* newDeclarationStatement(Declaration* declaration) {
    Statement* declarationStatement = newStatement(STATEMENT_DECLARATION);
    declarationStatement->Declaration = declaration;
    return declarationStatement;
}

Statement* newStatementBlock() {
    Statement* statementBlock = newStatement(STATEMENT_BLOCK);
    statementBlock->Block = calloc(1, sizeof(StatementBlock));
    statementBlock->Block->Statements = newStretchyBuffer(sizeof(Statement*));
    return statementBlock;
}

Statement* newIfStatement(Expression* condition, Statement* block, Statement* elseBlock) {
    Statement* ifStatement = newStatement(STATEMENT_IF);
    ifStatement->If.Condition = condition;
    ifStatement->If.Block = block;
    ifStatement->If.ElseBlock = elseBlock;
    return ifStatement;
}

Statement* newReturnStatement(Expression* expression) {
    Statement* returnStatement = newStatement(STATEMENT_RETURN);
    returnStatement->Expresssion = expression;
    return returnStatement;
}

void addStatement(StatementBlock* statementBlock, Statement* statement) {
    statementBlock->Count += 1;
    bufferPush(statementBlock->Statements, statement);
}

void addExpression(StatementBlock* statementBlock, Expression* expression) {
    addStatement(statementBlock, newExpressionStatement(expression));
}

void addDeclaration(StatementBlock* statementBlock, Declaration* declaration) {
    addStatement(statementBlock, newDeclarationStatement(declaration));
}
