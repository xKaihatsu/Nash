#ifndef STRETCHY_BUFFER_H
#define STRETCHY_BUFFER_H

#include <stddef.h>
#include <stdint.h>

typedef struct StretchyBuffer {
    size_t Length;
    size_t Capacity;
    uint8_t Buffer[];
} StretchyBuffer;

/*
    Get the internal header of a stretchy buffer.
*/
StretchyBuffer* stretchyBufferHeader(void* buffer);

/*
    Allocate a new stretchy buffer with its elements of size elementSize in bytes and return a pointer to it's memory.

    Returns NULL on failure.
*/
void* newStretchyBuffer(size_t elementSize);

/*
    Grow a stretchy buffer's memory by a factor of 1.5.

    Returns NULL when the buffer is NULL, elementSize is 0 or failure.
*/
void* stretchyBufferGrow(void* buffer, size_t elementSize);

/*
    Free a stretchy buffer.

    Does nothing when buffer is NULL.
*/
void freeStretchyBuffer(void* buffer);

#define bufferLength(buffer) (stretchyBufferHeader(buffer)->Length)
#define bufferCapacity(buffer) (stretchyBufferHeader(buffer)->Capacity)

#define bufferEnd(buffer) (buffer + stretchyBufferHeader(buffer)->Length)

#define bufferPush(buffer, value) \
        (bufferLength(buffer) >= bufferCapacity(buffer) ? buffer = stretchyBufferGrow(buffer, sizeof(*buffer)) : 0), \
        buffer[bufferLength(buffer)++] = (value)
        

#endif