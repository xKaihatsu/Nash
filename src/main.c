#include <stdio.h>
#include "Common.h"
#include "ReadFile.h"
#include "Lexer.h"
#include "Parser.h"
#include "Node.h"
#include "Generator.h"
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <stdarg.h>

static const char* asmExtension = ".asm";

void usage(const char* programName);

void sh(const char* command, ...) {
    va_list args;
    va_start(args, command);

    const char* args = 0;

    execlp(command, command, args, NULL);
    va_end(args);
}

int main(int argc, const char* argv[]) {
    const char* programName = argv[0];
    if (argc < 2) {
        usage(programName);
        return 0;
    }
    const char** arguments = argv + 1;
    const size_t argumentCount = argc - 1;
    const char* inputFilePath = NULL;
    const char* fileOutputPath = NULL;
    bool dumpAST = false;

    for (int i = 0; i < argumentCount; i++) {
        const char* argument = arguments[i];
        if (streq(argument, "ast")) {
            inputFilePath = arguments[i + 1];
            dumpAST = true;
        } else if (streq(argument, "build")) {
            inputFilePath = arguments[i + 1];
        } else if (streq(argument, "-o")) {
            fileOutputPath = arguments[i + 1];
        }
    }
    if (!fileOutputPath) {
        fileOutputPath = "output";
    }

    const char* file = readFile(inputFilePath);
    printf("%s:\n%s\n", inputFilePath, file);

    Lexer* lexer = newLexer(file);
    Token* tokens = scanTokens(lexer);
    //printTokens(lexer);

    sh("echo", "hello");

    /*
    const char* generatedAsmPath = strcat(strcpy(calloc(strlen(fileOutputPath) + strlen(asmExtension) + 1, sizeof(char)), fileOutputPath), asmExtension);

    Parser* parser = newParser(tokens);
    Node* program = parse(parser);
    Generator* generator = newGenerator(generatedAsmPath);
    generate(generator, program);
    */
}

void usage(const char* programName) {
    printf("Nash is a compiled language with its main influences being C, Typescript, and Lua.\n");
    printf("%s <command> arguments...\n", programName);
    printf("Commands:\n");
    printf("ast\t\tDisplays the abstract syntax tree of a given nash program.\n");
    printf("build\t\tCompiles given nash files.\n");
    printf("help\t\tDisplay this help message.\n");
}

