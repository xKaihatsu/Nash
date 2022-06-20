#include "ReadFile.h"
#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>

char* readFile(const char* filepath) {
    FILE* file = fopen(filepath, "r");
    if (!file)
        return NULL;
    
    fseek(file, 0, SEEK_END);
    size_t fileSize = ftell(file);
    fseek(file, 0, SEEK_SET);

    char* contents = calloc(fileSize + 1, sizeof(char));
    fread(contents, sizeof(char), fileSize, file);

    return contents;
}