///////////////////////////////////////////////////////////////////////////////
// NAME:            scanner.c
//
// AUTHOR:          Ethan D. Twardy <ethan.twardy@gmail.com>
//
// DESCRIPTION:     Implementation of the scanner.
//
// CREATED:         12/29/2021
//
// LAST EDITED:     12/29/2021
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

static const size_t BUFFER_SIZE = 4096;

typedef struct HbScanner {
    bool ws_enabled;
    HbInputContext* input_context;
    HbParseToken* token_cache;
    char* buffer;
    size_t buffer_index;
    size_t buffer_level;
} HbScanner;

///////////////////////////////////////////////////////////////////////////////
// Private API
////

static void priv_transfer_token(HbParseToken* dest, HbParseToken* source) {
    dest->type = source->type;
    dest->string = source->string;
    source->type = HB_TOKEN_NULL;
    source->string = NULL;
}

static char priv_next_char(HbScanner* scanner) {
    if (scanner->buffer_index >= scanner->buffer_level) {
        scanner->buffer_level = scanner->input_context->read(
            scanner->input_context->data, scanner->buffer, BUFFER_SIZE - 1);
        scanner->buffer_index = 0;
        scanner->buffer[scanner->buffer_level] = '\0';
    }

    return scanner->buffer[scanner->buffer_index++];
}

static int priv_ws_token(HbScanner* scanner, char current,
    HbParseToken* token)
{
    int result = 0;
    if ('\n' == current) {
        priv_transfer_token(token, scanner->token_cache);
        scanner->token_cache->type = HB_TOKEN_EOL;
        result = 1;
    } else

    if (scanner->ws_enabled) {
        // TODO
    }

    return result;
}

static int priv_handlebars_token(HbScanner* scanner, char current,
    HbParseToken* token)
{
    if ('{' != current && '}' != current) {
        return 0;
    }

    int result = 0;
    char next = priv_next_char(scanner);
    if (current == next) {
        priv_transfer_token(token, scanner->token_cache);
        scanner->token_cache->type = '{' == next ? HB_TOKEN_OPEN_BARS
            : HB_TOKEN_CLOSE_BARS;
        result = 1;
    } else if (priv_ws_token(scanner, next, token)) {
        result = 1;
    } else if ('\0' == next) {
        priv_transfer_token(token, scanner->token_cache);
        scanner->token_cache->type = HB_TOKEN_EOF;
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
    scanner->input_context = input_context;
    scanner->token_cache = malloc(sizeof(HbParseToken));
    if (NULL == scanner->token_cache) {
        free(scanner);
        return NULL;
    }
    memset(scanner->token_cache, 0, sizeof(HbParseToken));

    scanner->buffer = malloc(BUFFER_SIZE);
    if (NULL == scanner->buffer) {
        free(scanner->token_cache);
        free(scanner);
        return NULL;
    }
    memset(scanner->buffer, 0, BUFFER_SIZE);

    return scanner;
}

// Free internal memory assocaited with the scanner.
void hb_scanner_free(HbScanner* scanner) {
    free(scanner->buffer);
    free(scanner->token_cache);
    handlebars_input_context_free(&scanner->input_context);
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
    if (HB_TOKEN_NULL != scanner->token_cache->type) {
        priv_transfer_token(token, scanner->token_cache);
        return 1;
    }

    char current = 0;
    while ('\0' != (current = priv_next_char(scanner))) {
        // If matching "{{" or "}}"
        if (priv_handlebars_token(scanner, current, token) ||
            priv_ws_token(scanner, current, token)) {
            result = 1;
            break;
        }

        else {
            if (HB_TOKEN_TEXT != token->type) {
                token->type = HB_TOKEN_TEXT;
                token->string = hb_string_init();
                result = 1;
            }

            char fragment[] = {current, '\0'};
            hb_string_append_str(token->string, fragment);
        }
    }

    // Handle EOF condition:
    if ('\0' == current) {
        scanner->token_cache->type = HB_TOKEN_EOF;
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
    case HB_TOKEN_EOL:
        return "HB_TOKEN_EOL";
    case HB_TOKEN_EOF:
        return "HB_TOKEN_EOF";
    default:
        return "(null)";
    }
}

///////////////////////////////////////////////////////////////////////////////
