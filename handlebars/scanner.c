///////////////////////////////////////////////////////////////////////////////
// NAME:            scanner.c
//
// AUTHOR:          Ethan D. Twardy <ethan.twardy@gmail.com>
//
// DESCRIPTION:     Implementation of the scanner.
//
// CREATED:         12/29/2021
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
// HbScanner Private API
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
static inline void priv_init_text_token(HbParseToken* token,
    const HbScanner* scanner)
{
    token->type = HB_TOKEN_TEXT;
    token->line = scanner->line_count;
    token->column = scanner->column_count - 1;
    token->string = hb_string_init();
}

static inline void priv_init_open_bars_token(HbParseToken* token,
    const HbScanner* scanner)
{
    token->type = HB_TOKEN_OPEN_BARS;
    token->line = scanner->line_count;
    token->column = scanner->column_count - 2;
    token->string = NULL;
}

static inline void priv_init_close_bars_token(HbParseToken* token,
    const HbScanner* scanner)
{
    token->type = HB_TOKEN_CLOSE_BARS;
    token->line = scanner->line_count;
    token->column = scanner->column_count - 2;
    token->string = NULL;
}

static inline void priv_init_ws_token(HbParseToken* token,
    const HbScanner* scanner)
{
    token->type = HB_TOKEN_WS;
    token->line = scanner->line_count;
    token->column = scanner->column_count - 1;
    token->string = hb_string_init();
}

static inline void priv_init_eof_token(HbParseToken* token)
{ token->type = HB_TOKEN_EOF; }

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

// Handle the case of HB_TOKEN_WS. If <current> is a whitespace character, and
// whitespace token emission is enabled, generate a whitespace token and
// indicate to the caller that a token was generated (by returning 1).
static int priv_check_for_ws_token(HbScanner* scanner, char current) {
    if (!isspace(current) || !scanner->ws_enabled) {
        return 0;
    }

    HbParseToken* token = token_buffer_reserve(&scanner->token_buffer);
    priv_init_ws_token(token, scanner);
    while (isspace(current)) {
        char fragment[] = {current, '\0'};
        hb_string_append_str(token->string, fragment);
        current = priv_next_char(scanner);
    }

    return 1;
}

// Handle the case where this character is '{' or '}'. We have to consume
// another character from the stream, which ends up making token emission a
// little more complicated here. If we encountered a *_BARS token in the
// stream, signal this to the caller by returning 1.
static int priv_check_for_handlebars_token(HbScanner* scanner, char current) {
    if ('{' != current && '}' != current) {
        return 0;
    }

    int result = 0;
    char next = priv_next_char(scanner);
    if (current == next) {
        result = 1;
        HbParseToken* token = token_buffer_reserve(&scanner->token_buffer);
        switch (next) {
        case '{': priv_init_open_bars_token(token, scanner); break;
        case '}': priv_init_close_bars_token(token, scanner); break;
        default: assert(0); // Programmer's error.
        }
    } else if (priv_check_for_ws_token(scanner, next)) {
        result = 1;
    } else if ('\0' == next) {
        result = 1;
        priv_init_eof_token(token_buffer_reserve(&scanner->token_buffer));
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

    char current = 0;
    HbParseToken* text_token = NULL;
    while ('\0' != (current = priv_next_char(scanner))) {
        // If matching "{{" or "}}" or whitespace (if enabled)
        if (priv_check_for_handlebars_token(scanner, current) ||
            priv_check_for_ws_token(scanner, current)) {
            break;
        }

        else {
            if (NULL == text_token) {
                text_token = token_buffer_reserve(&scanner->token_buffer);
                priv_init_text_token(text_token, scanner);
            }

            char fragment[] = {current, '\0'};
            hb_string_append_str(text_token->string, fragment);
        }
    }

    // Handle EOF condition:
    if ('\0' == current) {
        HbParseToken* eof_token = token_buffer_reserve(&scanner->token_buffer);
        priv_init_eof_token(eof_token);
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
