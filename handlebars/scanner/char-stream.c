///////////////////////////////////////////////////////////////////////////////
// NAME:            char-stream.c
//
// AUTHOR:          Ethan D. Twardy <ethan.twardy@gmail.com>
//
// DESCRIPTION:     Implementation of the CharStream.
//
// CREATED:         12/30/2021
//
// LAST EDITED:     12/31/2021
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

#include <stdlib.h>
#include <string.h>

#include <handlebars/handlebars.h>
#include <handlebars/scanner/char-stream.h>

///////////////////////////////////////////////////////////////////////////////
// Public API
////

// Initialize a CharStream at <stream> with a peek buffer of <peek_buffer>,
// that is, allow peeking at chars up to `<peek_buffer> - 1` positions ahead of
// the cursor.
void char_stream_init(CharStream* stream, size_t capacity, size_t peek_length,
    HbInputContext* input_context)
{
    memset(stream, 0, sizeof(CharStream));
    stream->buffer = malloc(capacity);
    if (NULL == stream->buffer) {
        return;
    }

    memset(stream->buffer, 0, capacity);
    stream->input_context = input_context;
    stream->peek_length = peek_length;
    stream->capacity = capacity;
}

// Release internal memory held by the CharStream.
void char_stream_release(CharStream* stream)
{ free(stream->buffer); }

// Return the next char in the stream and increment the cursor. May assert if
// an error occurs in reading.
char char_stream_next(CharStream* stream) {
    if (stream->index >= stream->level) {
        stream->level = stream->input_context->read(
            stream->input_context->data, stream->buffer,
            stream->capacity - 1);
        stream->index = 0;
        stream->buffer[stream->level] = '\0';
    }

    return stream->buffer[stream->index++];
}

// Peek at the char <offset> positions ahead of the cursor. Will assert if
// <offset> is greater than the length of peek_buffer (as in char_stream_init
// above). May also assert if the attempt to peek kicks off a read request, and
// an error is occurred as a result.
char char_stream_peek(CharStream* stream, size_t offset) { return 0; }

///////////////////////////////////////////////////////////////////////////////
