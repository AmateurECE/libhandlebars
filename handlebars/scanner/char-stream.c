///////////////////////////////////////////////////////////////////////////////
// NAME:            char-stream.c
//
// AUTHOR:          Ethan D. Twardy <ethan.twardy@gmail.com>
//
// DESCRIPTION:     Implementation of the CharStream.
//
// CREATED:         12/30/2021
//
// LAST EDITED:     01/02/2022
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
#include <stdlib.h>
#include <string.h>

#include <handlebars/handlebars.h>
#include <handlebars/scanner/char-stream.h>

///////////////////////////////////////////////////////////////////////////////
// Private API
////

static void priv_fill_buffer_with_peek_shift(CharStream* stream) {
    for (size_t i = 0; i < stream->peek_length; ++i) {
        stream->buffer[i] = stream->buffer[stream->index + i];
    }

    stream->level = stream->input_context->read(
        stream->input_context->data, stream->buffer + stream->peek_length,
        stream->capacity - stream->peek_length - 1);
    stream->index = 0;
    stream->buffer[stream->level + stream->peek_length] = '\0';
}

static void priv_fill_buffer(CharStream* stream) {
    stream->level = stream->input_context->read(
        stream->input_context->data, stream->buffer,
        stream->capacity - 1);
    stream->index = 0;
    stream->buffer[stream->level] = '\0';
}

///////////////////////////////////////////////////////////////////////////////
// Public API
////

// Initialize a CharStream at <stream> with a peek buffer of <peek_buffer>,
// that is, allow peeking at chars up to `<peek_buffer> - 1` positions ahead of
// the cursor. Will assert if <peek_length> is greater than <capacity>.
void char_stream_init(CharStream* stream, size_t capacity, size_t peek_length,
    HbInputContext* input_context)
{
    assert(peek_length <= capacity);
    memset(stream, 0, sizeof(CharStream));
    stream->buffer = malloc(capacity);
    if (NULL == stream->buffer) {
        return;
    }

    memset(stream->buffer, 0, capacity);
    stream->input_context = input_context;
    stream->peek_length = peek_length;
    stream->capacity = capacity;
    priv_fill_buffer(stream);
}

// Release internal memory held by the CharStream.
void char_stream_release(CharStream* stream)
{ free(stream->buffer); }

// Return the next char in the stream and increment the cursor. May assert if
// an error occurs in reading.
char char_stream_next(CharStream* stream) {
    // May need to read more from the input source.
    if (stream->index >= stream->level - stream->peek_length) {
        priv_fill_buffer_with_peek_shift(stream);
    }

    return stream->buffer[stream->index++];
}

// Peek at the char <offset> positions ahead of the cursor. Will assert if
// <offset> is greater than the length of peek_buffer (as in char_stream_init
// above).
char char_stream_peek(const CharStream* stream, size_t offset) {
    assert(offset <= stream->peek_length);
    return stream->buffer[stream->index + offset];
}

///////////////////////////////////////////////////////////////////////////////
