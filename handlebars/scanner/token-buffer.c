///////////////////////////////////////////////////////////////////////////////
// NAME:            token-buffer.c
//
// AUTHOR:          Ethan D. Twardy <ethan.twardy@gmail.com>
//
// DESCRIPTION:     Implementation of the TokenBuffer.
//
// CREATED:         12/30/2021
//
// LAST EDITED:     01/07/2022
//
// Copyright 2021, Ethan D. Twardy
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to
// deal in the Software without restriction, including without limitation the
// rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
// sell copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
// FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
// IN THE SOFTWARE.
////

#include <assert.h>
#include <errno.h>
#include <stdlib.h>
#include <string.h>

#include <handlebars/scanner.h>
#include <handlebars/scanner/token-buffer.h>

///////////////////////////////////////////////////////////////////////////////
// Public API
////

// Initialize a TokenBuffer at <buffer>.
int token_buffer_init(TokenBuffer* buffer, size_t buffer_length) {
    memset(buffer, 0, sizeof(TokenBuffer));
    buffer->buffer = calloc(buffer_length, sizeof(HbsParseToken));
    if (NULL == buffer->buffer) {
        return ENOMEM;
    }

    buffer->capacity = buffer_length;
    return 0;
}

// Release memory held internally by a TokenBuffer.
void token_buffer_release(TokenBuffer* buffer)
{ free(buffer->buffer); }

// Reserve a new slot in the ring buffer and return a pointer to it. This
// allows in-place construction of HbsParseTokens. Since we aim to set the size
// of this buffer never to overflow, just assert if overflow is detected.
HbsParseToken* token_buffer_reserve(TokenBuffer* buffer) {
    assert(buffer->length < buffer->capacity);
    HbsParseToken* token = &buffer->buffer[buffer->top];
    buffer->length += 1;
    buffer->top = (buffer->top + 1) % buffer->capacity;
    return token;
}

// De-queue a token from the bottom of the buffer (return a pointer to it) and
// move the cursor to the next token in the buffer.
HbsParseToken* token_buffer_dequeue(TokenBuffer* buffer) {
    if (0 == buffer->length) {
        return NULL;
    }

    HbsParseToken* token = &buffer->buffer[buffer->bottom];
    buffer->length -= 1;
    buffer->bottom = (buffer->bottom + 1) % buffer->capacity;
    return token;
}

// Peek at the token that would be popped with a call to _dequeue().
HbsParseToken* token_buffer_peek(TokenBuffer* buffer) {
    if (0 == buffer->length) {
        return NULL;
    }

    return &buffer->buffer[buffer->bottom];
}

///////////////////////////////////////////////////////////////////////////////
