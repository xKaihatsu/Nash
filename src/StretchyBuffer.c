#include "StretchyBuffer.h"
#include <stdlib.h>
#include <string.h>
#include <stddef.h>
#include <stdint.h>

#define INITIAL_BUFFER_CAPACITY 4

StretchyBuffer* stretchyBufferHeader(void* buffer) {
    return (StretchyBuffer*)((uint8_t*)buffer - offsetof(StretchyBuffer, Buffer));
}


void* newStretchyBuffer(size_t elementSize) {
    StretchyBuffer* stretchyBuffer = calloc(1, sizeof(StretchyBuffer) + INITIAL_BUFFER_CAPACITY * elementSize);
    stretchyBuffer->Capacity = INITIAL_BUFFER_CAPACITY;
    if (!stretchyBuffer) return NULL;

    return stretchyBuffer->Buffer;
}


void* stretchyBufferGrow(void* buffer, size_t elementSize) {
    if (!buffer || !elementSize) return NULL;
    StretchyBuffer* stretchyBuffer = stretchyBufferHeader(buffer);

    size_t length = stretchyBuffer->Length;
    size_t newCapacity = stretchyBuffer->Capacity * 1.5;
    StretchyBuffer* newBuffer = calloc(1, sizeof(StretchyBuffer) + newCapacity * elementSize);
    if (!newBuffer) return NULL;

    newBuffer->Length = length;
    newBuffer->Capacity = newCapacity;
    memcpy(newBuffer->Buffer, stretchyBuffer->Buffer, length * elementSize);

    free(stretchyBuffer);
    return newBuffer->Buffer;
}

void freeStretchyBuffer(void* buffer) {
    if (!buffer) return;
    free(stretchyBufferHeader(buffer));
}