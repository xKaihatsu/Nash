#include "Node.h"
#include "StretchyBuffer.h"
#include <stdio.h>
#include <stdlib.h>

static Node* newNode(NodeType type) {
    Node* node = calloc(1, sizeof(Node));
    node->Type = type;
    return node;
}

Node* newExpressionNode(Expression* expression) {
    Node* node = newNode(NODE_EXPRESSION);
    node->Expression = expression;
    return node;
}
Node* newDeclarationNode(Declaration* declaration) {
    Node* node = newNode(NODE_DECLARATION);
    node->Declaration = declaration;
    return node;
}
Node* newStatementNode(Statement* statement) {
    Node* node = newNode(NODE_STATEMENT);
    node->Statement = statement;
    return node;
}

Node* newProgramNode() {
    Node* node = newNode(NODE_PROGRAM);
    node->Program = calloc(1, sizeof(ProgramNode));
    node->Program->Count = 0;
    node->Program->Nodes = newStretchyBuffer(sizeof(Node*));
    return node;
}

void addNode(ProgramNode* program, Node* node) {
    program->Count += 1;
    bufferPush(program->Nodes, node);
}

static void printIndentation(FILE* stream, unsigned int indentation) {
    for (int i = 0; i < indentation; i++) {
        fprintf(stream, "\t");
    }
}

void dumpExpression(FILE* stream, Expression* expression, unsigned int indentation) {
    switch (expression->Type) {
        case EXPRESSION_LITERAL: {
            printIndentation(stream, indentation);
            Literal literal = expression->Literal;
            switch (literal.Type) {
                case LITERAL_INTEGER: fprintf(stream, "%lu", literal.Integer); break;
                case LITERAL_FLOAT: fprintf(stream, "%g", literal.Float); break;
            }
        } break;
        case EXPRESSION_BINARY: {
            BinaryExpression binary = expression->Binary;
            printIndentation(stream, indentation);
            fprintf(stream, "{\n");
            printIndentation(stream, indentation + 1);
            fprintf(stream, "\"Operation\": \"%s\",\n", OPERATION_TO_STRING[binary.Operation]);
            printIndentation(stream, indentation + 1);
            fprintf(stream, "\"Left\": ");
            dumpExpression(stream, binary.Left, indentation + 1);
            fprintf(stdout, ",\n");
            printIndentation(stream, indentation + 1);
            fprintf(stream, "\"Right\": ");
            dumpExpression(stream, binary.Right, indentation + 1);
            fprintf(stdout, "\n");
            printIndentation(stream, indentation);
            fprintf(stream, "}");
        } break;
        case EXPRESSION_CALL: {
            FunctionCall call = expression->Call;
            printIndentation(stream, indentation);
            fprintf(stream, "{\n");
            printIndentation(stream, indentation + 1);
            fprintf(stream, "\"Name\": \"%s\",\n", call.Name);
            printIndentation(stream, indentation + 1);
            fprintf(stream, "\"Arguments\": [%s", call.Arity > 0 ? "\n" : "\0");
            for (size_t i = 0; i < call.Arity; i++) {
                Expression* argument = call.Arguments[i];
                dumpExpression(stream, argument, indentation + 2);
                fprintf(stream, "%s", i + 1 < call.Arity ? ",\n" : "\0");
            }
            fprintf(stream, "\n");
            printIndentation(stdout, indentation + 1);
            fprintf(stream, "]\n");
            printIndentation(stream, indentation);
            fprintf(stream, "}");
        } break;
        case EXPRESSION_VARIABLE: {
            Expression* variable = expression;
            printIndentation(stream, indentation);
            fprintf(stream, "\"%s\"", variable->Variable);
        } break;
    }
}

void dumpDeclaration(FILE* stream, Declaration* declaration, unsigned int indentation) {
    switch (declaration->Type) {
        case DECLARATION_VARIABLE: {
            VariableDeclaration variableDeclaration = declaration->Variable;
            printIndentation(stream, indentation);
            fprintf(stream, "\"VariableDeclaration\": {\n");
            printIndentation(stream, indentation + 1);
            fprintf(stream, "\"Name\": \"%s\"%s\n", declaration->Name, variableDeclaration.Initializer ? "," : "\0");
            if (variableDeclaration.Initializer) {
                printIndentation(stream, indentation + 1);
                fprintf(stream, "\"Value\": ");
                dumpExpression(stream, variableDeclaration.Initializer, indentation + 1);
                fprintf(stream, "\n");
            }
            printIndentation(stream, indentation);
            fprintf(stream, "}");
        } break;
        case DECLARATION_FUNCTION: {
            FunctionDeclaration functionDeclaration = declaration->Function;
            printIndentation(stream, indentation);
            fprintf(stream, "\"FunctionDeclaration\": {\n");
            printIndentation(stream, indentation + 1);
            fprintf(stream, "\"Name\": \"%s\",\n", declaration->Name);
            printIndentation(stream, indentation + 1);
            fprintf(stream, "\"Parameters\": [");
            if (functionDeclaration.Arity > 0) {
                fprintf(stream, "\n");
                for (size_t i = 0; i < functionDeclaration.Arity; i++) {
                    Declaration* parameter = functionDeclaration.Parameters[i];
                    dumpDeclaration(stream, parameter, indentation + 2);
                }
                fprintf(stream, "\n");
                printIndentation(stream, indentation + 1);
            }
            fprintf(stream, "],\n");
            printIndentation(stream, indentation + 1);
            fprintf(stream, "\"Block\": ");
            dumpStatement(stream, functionDeclaration.Block, indentation + 1);
            printf("\n");
            printIndentation(stream, indentation);
            fprintf(stream, "}");
        }
    }
}

void dumpStatement(FILE* stream, Statement* statement, unsigned int indentation) {
    switch (statement->Type) {
        case STATEMENT_EXPRESSION: dumpExpression(stream, statement->Expresssion, indentation); break;
        case STATEMENT_DECLARATION: dumpDeclaration(stream, statement->Declaration, indentation); break;
        case STATEMENT_BLOCK: {
            StatementBlock* block = statement->Block;
            fprintf(stream, "[");
            if (block->Count > 0) {
                fprintf(stream, "\n");
                for (size_t i = 0; i < block->Count; i++) {
                    Statement* statement = block->Statements[i];
                    printIndentation(stream, indentation + 2);
                    fprintf(stream, "{\n");
                    dumpStatement(stream, statement, indentation + 3);
                    fprintf(stream, "\n");
                    printIndentation(stream, indentation + 2);
                    fprintf(stream, "}%s\n", i + 1 != block->Count ? "," : "\0");
                }
                printIndentation(stream, indentation + 1);
            }
            fprintf(stream, "]");
        } break;
        case STATEMENT_IF: {
            IfStatement ifStatement = statement->If;
            fprintf(stream, "\"If\": {\n");
            printIndentation(stream, indentation + 1);
            fprintf(stream, "\"Condition\": ");
            dumpExpression(stream, ifStatement.Condition, indentation + 1);
            fprintf(stream, ",\n");
            printIndentation(stream, indentation + 1);
            fprintf(stream, "\"Block\": ");
            dumpStatement(stream, ifStatement.Block, indentation);
            if (ifStatement.ElseBlock) {
                fprintf(stream, ",\n");
                fprintf(stream, "\"Else\": ");
                dumpStatement(stream, ifStatement.ElseBlock, indentation);
            } else {
                fprintf(stream, "\n");
            }
            printIndentation(stream, indentation);
            fprintf(stream, "}");
        } break;
        case STATEMENT_RETURN: {
            printIndentation(stream, indentation);
            fprintf(stream, "\"Return\": ");
            dumpExpression(stream, statement->Expresssion, indentation);
        } break;
    }
}

void dumpNode(FILE* stream, Node* node) {
    switch (node->Type) {
        case NODE_EXPRESSION: dumpExpression(stream, node->Expression, 1); break;
        case NODE_DECLARATION: dumpDeclaration(stream, node->Declaration, 1); break;
        case NODE_STATEMENT: dumpStatement(stream, node->Statement, 1); break;
        case NODE_PROGRAM: {
            printf("{\n");
            ProgramNode* program = node->Program;
            for (int i = 0; i < program->Count; i++) {
                Node* node = program->Nodes[i];
                dumpNode(stream, node);
                fprintf(stream, "%s", i + 1 != program->Count ? ",\n" : "\0");
            }
            printf("\n}\n");
        } break;
    }
}
