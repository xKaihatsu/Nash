#ifndef GENERATOR_H
#define GENERATOR_H

#include "Common.h"
#include "Node.h"
#include <stdio.h>

typedef struct Generator {
    FILE* Output;
} Generator;

Generator* newGenerator(const char* filepath);
void freeGenerator(Generator* generator);

void generate(Generator* generator, Node* node);

#endif