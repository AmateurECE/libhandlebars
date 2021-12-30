///////////////////////////////////////////////////////////////////////////////
// NAME:            scanner.h
//
// AUTHOR:          Ethan D. Twardy <ethan.twardy@gmail.com>
//
// DESCRIPTION:     Interface for a scanner
//
// CREATED:         12/28/2021
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

#ifndef HANDLEBARS_SCANNER_H
#define HANDLEBARS_SCANNER_H

// Opaque typedef of the HbScanner type.
typedef struct HbScanner HbScanner;

// Forward declarations.
typedef struct HbInputContext HbInputContext;
typedef struct HbString HbString;

// Valid parser tokens.
typedef enum HbParseTokenType {
    HB_TOKEN_NULL,          // '\0', or a named constant to signal no token
    HB_TOKEN_OPEN_BARS,     // "{{"
    HB_TOKEN_CLOSE_BARS,    // "}}"

    // This token is generated whenever interesting text is discovered. If the
    // ws token is enabled, this token is generated for word input, i.e. [\w]+,
    // but is more akin to .+ whenever the ws token is disabled.
    HB_TOKEN_TEXT,

    // This token may or may not be generated, based on the value of an
    // internal boolean, which can be enabled or disabled with the *_ws_token
    // functions. See their description for more info.
    HB_TOKEN_WS,            // [ \t\n]*

    HB_TOKEN_EOF, // End of file (or stream)

    // End of line (always generated, regardless of ws enable/disable state).
    HB_TOKEN_EOL,
} HbParseTokenType;

// Structure of a token event.
typedef struct HbParseToken {
    HbParseTokenType type;
    HbString* string;
} HbParseToken;

// Create a new HbScanner.
HbScanner* hb_scanner_new(HbInputContext* input_context);

// These functions enable or disable the "whitespace" token. When we're not
// parsing a handlebars expression, whitespace is signifigcant, so we want it
// to remain intact with the text, but there's also no reason to interrupt the
// parser to notify it of whitespace. This was an interesting design choice
// aimed at simplifying the logic of the scanner at the expense of a slightly
// more complex interface.
void hb_scanner_disable_ws_token(HbScanner* scanner);
void hb_scanner_enable_ws_token(HbScanner* scanner);

// Populate <token> with the next token from the stream. Return the number of
// tokens processed (i.e. 1 for a successful scan).
int hb_scanner_next_symbol(HbScanner* scanner, HbParseToken* token);

// Free internal memory assocaited with the scanner.
void hb_scanner_free(HbScanner* scanner);

// Return a string describing the Parser token type (for debugging purposes)
const char* hb_token_to_string(HbParseTokenType type);

#endif // HANDLEBARS_SCANNER_H

///////////////////////////////////////////////////////////////////////////////
