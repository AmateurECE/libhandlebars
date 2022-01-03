///////////////////////////////////////////////////////////////////////////////
// NAME:            scanner.c
//
// AUTHOR:          Ethan D. Twardy <ethan.twardy@gmail.com>
//
// DESCRIPTION:     Implementation of the scanner.
//
// CREATED:         12/29/2021
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
#include <ctype.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

#include <handlebars/handlebars.h>
#include <handlebars/scanner.h>
#include <handlebars/scanner/char-stream.h>
#include <handlebars/scanner/token-buffer.h>

static const size_t CHAR_BUFFER_SIZE = 4096;
static const size_t TOKEN_BUFFER_SIZE = 8;
static const size_t PEEK_LENGTH = 2;

typedef struct HbScanner {
    // If this flag is true, the scanner treats whitespace as a separate token
    // and generates HB_TOKEN_WS instances. Otherwise, all whitespace is
    // considered to be part of a HB_TOKEN_TEXT token.
    bool ws_enabled;

    // Provides a buffered, character-based input stream.
    CharStream stream;

    // Keep track of the line and column of the cursor
    int line_count;
    int column_count;

    // Output stream for the tokens.
    TokenBuffer token_buffer;
} HbScanner;

///////////////////////////////////////////////////////////////////////////////
// Token Conveniences
////

// "Move" the data from <source> to <dest>, destroying <source>.
static void priv_move_token(HbParseToken* dest, HbParseToken* source) {
    dest->type = source->type;
    dest->string = source->string;
    dest->line = source->line;
    dest->column = source->column;
    memset(source, 0, sizeof(HbParseToken));
}

// Initialize a token of the given type, borrowing some context from the
// scanner.
static inline void priv_init_token(HbParseTokenType type, HbParseToken* token,
    const HbScanner* scanner)
{
    token->type = type;
    token->line = scanner->line_count;
    token->column = scanner->column_count;
    token->string = NULL;
}

static inline void priv_init_text_token(HbParseToken* token,
    const HbScanner* scanner)
{
    priv_init_token(HB_TOKEN_TEXT, token, scanner);
    token->string = hb_string_init();
}

static inline void priv_init_open_bars_token(HbParseToken* token,
    const HbScanner* scanner)
{ priv_init_token(HB_TOKEN_OPEN_BARS, token, scanner); }

static inline void priv_init_close_bars_token(HbParseToken* token,
    const HbScanner* scanner)
{ priv_init_token(HB_TOKEN_CLOSE_BARS, token, scanner); }

static inline void priv_init_ws_token(HbParseToken* token,
    const HbScanner* scanner)
{
    priv_init_token(HB_TOKEN_WS, token, scanner);
    token->string = hb_string_init();
}

static inline void priv_init_eof_token(HbParseToken* token,
    const HbScanner* scanner)
{ priv_init_token(HB_TOKEN_EOF, token, scanner); }

///////////////////////////////////////////////////////////////////////////////
// Private HbScanner Support Functions
////

// Obtain the next char from the buffered stream, which could result in reading
// more data from the stream.
static char priv_next_char(HbScanner* scanner) {
    char result = char_stream_next(&scanner->stream);
    if ('\n' == result) {
        scanner->line_count += 1;
        scanner->column_count = 0;
    } else {
        scanner->column_count += 1;
    }

    return result;
}

static bool priv_is_ws_token(const CharStream* stream)
{ return isspace(char_stream_peek(stream, 0)); }

static void priv_consume_ws_token(HbScanner* scanner) {
    HbParseToken* token = token_buffer_reserve(&scanner->token_buffer);
    priv_init_ws_token(token, scanner);

    char current = char_stream_peek(&scanner->stream, 0);
    while (isspace(current)) {
        char fragment[] = {current, '\0'};
        hb_string_append_str(token->string, fragment);
        priv_next_char(scanner);
        current = char_stream_peek(&scanner->stream, 0);
    }
}

// Return true if the next token in the stream is
static bool priv_is_handlebars_token(const CharStream* stream) {
    char current = char_stream_peek(stream, 0);
    char next = char_stream_peek(stream, 1);
    return ('{' == current || '}' == current) && current == next;
}

static void priv_consume_handlebars_token(HbScanner* scanner) {
    // Have to consume two tokens from the stream.
    char current = char_stream_peek(&scanner->stream, 0);
    HbParseToken* token = token_buffer_reserve(&scanner->token_buffer);
    switch (current) {
    case '{': priv_init_open_bars_token(token, scanner); break;
    case '}': priv_init_close_bars_token(token, scanner); break;
    default: assert(0); // Programmer's error.
    }

    // Have to consume the chars in the token after initializing the token,
    // since token initialization captures the line/column counts.
    priv_next_char(scanner);
    priv_next_char(scanner);
}

static bool priv_is_eof_token(const CharStream* stream)
{ return '\0' == char_stream_peek(stream, 0); }

static void priv_consume_eof_token(HbScanner* scanner) {
    HbParseToken* token = token_buffer_reserve(&scanner->token_buffer);
    priv_init_eof_token(token, scanner);
}

static int priv_iterate_lexer(HbScanner* scanner) {
    int result = 0;
    if (priv_is_handlebars_token(&scanner->stream)) {
        priv_consume_handlebars_token(scanner);
        result = 1;
    } else if (scanner->ws_enabled && priv_is_ws_token(&scanner->stream)) {
        priv_consume_ws_token(scanner);
        result = 1;
    } else if (priv_is_eof_token(&scanner->stream)) {
        priv_consume_eof_token(scanner);
        result = 1;
    }

    return result;
}

///////////////////////////////////////////////////////////////////////////////
// Public API
////

// Create a new HbScanner.
HbScanner* hb_scanner_new(HbInputContext* input_context) {
    HbScanner* scanner = malloc(sizeof(HbScanner));
    if (NULL == scanner) {
        return NULL;
    }

    memset(scanner, 0, sizeof(HbScanner));
    scanner->line_count = 1;
    token_buffer_init(&scanner->token_buffer, TOKEN_BUFFER_SIZE);
    char_stream_init(&scanner->stream, CHAR_BUFFER_SIZE, PEEK_LENGTH,
        input_context);

    return scanner;
}

// Free internal memory assocaited with the scanner.
void hb_scanner_free(HbScanner* scanner) {
    char_stream_release(&scanner->stream);
    token_buffer_release(&scanner->token_buffer);
    free(scanner);
}

// These functions enable or disable the "whitespace" token. When we're not
// parsing a handlebars expression, whitespace is signifigcant, so we want it
// to remain intact with the text, but there's also no reason to interrupt the
// parser to notify it of whitespace. This was an interesting design choice
// aimed at simplifying the logic of the scanner at the expense of a slightly
// more complex interface.
void hb_scanner_disable_ws_token(HbScanner* scanner)
{ scanner->ws_enabled = false; }
void hb_scanner_enable_ws_token(HbScanner* scanner)
{ scanner->ws_enabled = true; }

// Populate <token> with the next token from the stream. Return the number of
// tokens processed (i.e. 1 for a successful scan).
// Token table:
int hb_scanner_next_symbol(HbScanner* scanner, HbParseToken* token) {
    int result = 0;
    if (0 < scanner->token_buffer.length) {
        HbParseToken* next = token_buffer_dequeue(&scanner->token_buffer);
        priv_move_token(token, next);
        return 1;
    }

    HbParseToken* text_token = NULL;
    while (!priv_iterate_lexer(scanner)) {
        if (NULL == text_token) {
            text_token = token_buffer_reserve(&scanner->token_buffer);
            priv_init_text_token(text_token, scanner);
        }

        char fragment[] = {priv_next_char(scanner), '\0'};
        hb_string_append_str(text_token->string, fragment);
    }

    if (0 < scanner->token_buffer.length) {
        HbParseToken* next = token_buffer_dequeue(&scanner->token_buffer);
        priv_move_token(token, next);
        result = 1;
    }

    return result;
}

// Return a string describing the Parser token type (for debugging purposes)
const char* hb_token_to_string(HbParseTokenType type) {
    switch (type) {
    case HB_TOKEN_OPEN_BARS:
        return "HB_TOKEN_OPEN_BARS";
    case HB_TOKEN_CLOSE_BARS:
        return "HB_TOKEN_CLOSE_BARS";
    case HB_TOKEN_TEXT:
        return "HB_TOKEN_TEXT";
    case HB_TOKEN_WS:
        return "HB_TOKEN_WS";
    case HB_TOKEN_EOF:
        return "HB_TOKEN_EOF";
    default:
        return "(null)";
    }
}

// Release internal memory held by <token>. This allows the caller to manage
// the memory of <token> itself.
void hb_token_release(HbParseToken* token) {
    if (HB_TOKEN_TEXT == token->type || HB_TOKEN_WS == token->type) {
        hb_string_free(&token->string);
    }

    memset(token, 0, sizeof(HbParseToken));
}

///////////////////////////////////////////////////////////////////////////////
