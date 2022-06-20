#ifndef COMMON_H
#define COMMON_H

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

#define boolToString(condition) ((condition) ? "true" : "false")

bool streq(const char* a, const char* b);
bool strneq(const char* a, const char* b, size_t length);

#endif