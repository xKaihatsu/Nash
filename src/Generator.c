#include "Generator.h"
#include "Node.h"
#include <stdio.h>
#include <stdlib.h>

static void generateExpression(Generator* generator, Expression* expression);
static void generateStatement(Generator* generator, Statement* statement);
static void generateDeclaration(Generator* generator, Declaration* declaration);

Generator* newGenerator(const char* filepath) {
    Generator* generator = calloc(1, sizeof(Generator));
    generator->Output = fopen(filepath, "w");
    return generator;
}

void freeGenerator(Generator* generator) {
    free(generator);
}

static void generatePreamble(Generator* generator) {
    fputs( 
    "global _start\n"
    "_start:\n"
    "\tmov rbp, rsp\n"
    "\tmov rdi, rsi\n"
    "\n"
    "\tmov rdi, [rbp]\n"
    "\tlea rsi, [rbp + 8]\n"
    "\tpush rdi\n"
    "\tpush rsi\n"
    "\tcall main\n"
    "\tmov rdi, rax\n"
    "\tmov rax, 60\n"
    "\tsyscall\n"
    "printInteger:\n"
    "\tpush rbp\n"
    "\tmov rbp, rsp\n"
    "\tmov rdi, [rbp + 16]\n"
    "\ttest rdi, rdi\n"
    "\tjge .L1\n"
    "\tpush rdi\n"
    "\tpush '-'\n"
    "\tcall printCharacter\n"
    "\tpop rdi\n"
    "\tneg rdi\n"
    ".L1:\n"
    "\tmov rax, rdi\n"
    "\tmov rsi, 10\n"
    "\tcqo\n"
    "\tidiv rsi\n"
    "\tmov rdi, rdx\n"
    "\ttest rax, rax\n"
    "\tjz .L2\n"
    "\tpush rdi\n"
    "\tpush rax\n"
    "\tcall printInteger\n"
    "\tpop rdi\n"
    ".L2:\n"
    "\tadd rdi, '0'\n"
    "\tpush rdi\n"
    "\tcall printCharacter\n"
    "\tpop rbp\n"
    "\tret 8\n"
    , generator->Output);
}
static void generatePostamble(Generator* generator) {}

static void generateFunction(Generator* generator, Declaration* functionDeclaration) {
    FunctionDeclaration function = functionDeclaration->Function;

    fprintf(generator->Output, "%s:\n", functionDeclaration->Name);
    fputs(
        "\tpush rbp\n"
        "\tmov rbp, rsp\n"
    , generator->Output);
    StatementBlock* block = function.Block->Block;
    for (int i = 0; i < block->Count; i++) {
        Statement* statement = block->Statements[i];
        generateStatement(generator, statement);
    }

    fputs(
        "\tmov rsp, rbp\n"
        "\tpop rbp\n"
    , generator->Output);
    if (function.Arity > 0) {
        fprintf(generator->Output, "\tret %lu\n", function.Arity * 8);
    } else {
        fprintf(generator->Output, "\tret\n");
    }
}

static void generateFunctionCall(Generator* generator, FunctionCall call) {
    for (int i = 0; i < call.Arity; i++) {
        Expression* argument = call.Arguments[i];
        switch (argument->Type) {
            case EXPRESSION_LITERAL: {
                Literal literal = argument->Literal;
                switch (literal.Type) {
                    case LITERAL_INTEGER: {
                        fprintf(generator->Output, "\tpush %lu\n", literal.Integer);
                    } break;
                }
            } break;
        } break;
    }
    fprintf(generator->Output, "\tcall %s\n", call.Name);
}

static void generateExpression(Generator* generator, Expression* expression) {
    switch (expression->Type) {
        case EXPRESSION_CALL: {
            generateFunctionCall(generator, expression->Call);
        } break;
    }
}

static void generateDeclaration(Generator* generator, Declaration* declaration) {
    switch (declaration->Type) {
        case DECLARATION_FUNCTION: {
            generateFunction(generator, declaration);
        } break;
    }
}

static void generateStatement(Generator* generator, Statement* statement) {
    switch (statement->Type) {
        case STATEMENT_DECLARATION: {
            generateDeclaration(generator, statement->Declaration);
        } break;
        case STATEMENT_EXPRESSION: {
            generateExpression(generator, statement->Expresssion);
        } break;
    }
}

static void generateNode(Generator* generator, Node* node) {
    switch (node->Type) {
        case NODE_STATEMENT: {
            generateStatement(generator, node->Statement);
        } break;
        case NODE_DECLARATION: {
            generateDeclaration(generator, node->Declaration);
        } break;
        case NODE_PROGRAM: {
            ProgramNode* program = node->Program;
            for (size_t i = 0; i < program->Count; i++) {
                Node* node = program->Nodes[i];
                generateNode(generator, node);
            }
        } break;
    }
}

void generate(Generator* generator, Node* node) {
    generatePreamble(generator);
    generateNode(generator, node);
}