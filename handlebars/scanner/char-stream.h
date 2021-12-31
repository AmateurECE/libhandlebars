///////////////////////////////////////////////////////////////////////////////
// NAME:            char-stream.h
//
// AUTHOR:          Ethan D. Twardy <ethan.twardy@gmail.com>
//
// DESCRIPTION:     Interface that allows reading a single char at a time from
//                  an HbInputContext and peeking up to two chars ahead of the
//                  cursor.
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

#ifndef HANDLEBARS_CHAR_STREAM_H
#define HANDLEBARS_CHAR_STREAM_H

#include <stddef.h>

typedef struct HbInputContext HbInputContext;

typedef struct CharStream {
    // Provides a stream of input characters. May be buffered, but it doesn't
    // really matter.
    HbInputContext* input_context;

    // Buffered stream state.
    char* buffer;
    size_t index;
    size_t level;
    size_t peek_length;
    size_t capacity;
} CharStream;

// Initialize a CharStream at <stream> with a peek buffer of <peek_buffer>,
// that is, allow peeking at chars up to `<peek_buffer> - 1` positions ahead of
// the cursor.
void char_stream_init(CharStream* stream, size_t capacity, size_t peek_buffer,
    HbInputContext* input_context);

// Release internal memory held by the CharStream.
void char_stream_release(CharStream* stream);

// Return the next char in the stream and increment the cursor. May assert if
// an error occurs in reading.
char char_stream_next(CharStream* stream);

// Peek at the char <offset> positions ahead of the cursor. Will assert if
// <offset> is greater than the length of peek_buffer (as in char_stream_init
// above). May also assert if the attempt to peek kicks off a read request, and
// an error is occurred as a result.
char char_stream_peek(CharStream* stream, size_t offset);

#endif // HANDLEBARS_CHAR_STREAM_H

///////////////////////////////////////////////////////////////////////////////
