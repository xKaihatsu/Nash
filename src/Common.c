#include "Common.h"
#include <string.h>


bool streq(const char* a, const char* b) {
    size_t aLength = strlen(a);
    size_t bLength = strlen(b);
    return aLength == bLength && strcmp(a, b) == 0;
}
bool strneq(const char* a, const char* b, size_t length) {
    return strncmp(a, b, length) == 0;
}