///////////////////////////////////////////////////////////////////////////////
// NAME:            token-buffer.h
//
// AUTHOR:          Ethan D. Twardy <ethan.twardy@gmail.com>
//
// DESCRIPTION:     Ring buffer for parser tokens.
//
// CREATED:         12/30/2021
//
// LAST EDITED:     12/30/2021
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

#ifndef HANDLEBARS_TOKEN_BUFFER_H
#define HANDLEBARS_TOKEN_BUFFER_H

typedef struct TokenBuffer {
    size_t capacity;        // Total capacity of the buffer
    size_t length;          // Number of items currently in the buffer
    size_t top;             // Array index pointing to current "top"
    size_t bottom;          // Array index pointing to current "bottom"
    HbParseToken* buffer;   // Array of HbParseTokens.
} TokenBuffer;

// Initialize a TokenBuffer at <buffer>.
int token_buffer_init(TokenBuffer* buffer, size_t buffer_length);

// Release memory held internally by a TokenBuffer.
void token_buffer_release(TokenBuffer* buffer);

// Reserve a new slot in the ring buffer and return a pointer to it. This
// allows in-place construction of HbParseTokens. Since we aim to set the size
// of this buffer never to overflow, just assert if overflow is detected.
HbParseToken* token_buffer_reserve(TokenBuffer* buffer);

// De-queue a token from the bottom of the buffer (return a pointer to it) and
// move the cursor to the next token in the buffer.
HbParseToken* token_buffer_dequeue(TokenBuffer* buffer);

#endif // HANDLEBARS_TOKEN_BUFFER_H

///////////////////////////////////////////////////////////////////////////////
