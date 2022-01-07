///////////////////////////////////////////////////////////////////////////////
// NAME:            scanner.h
//
// AUTHOR:          Ethan D. Twardy <ethan.twardy@gmail.com>
//
// DESCRIPTION:     Interface for a scanner
//
// CREATED:         12/28/2021
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

#ifndef HANDLEBARS_SCANNER_H
#define HANDLEBARS_SCANNER_H

// Opaque typedef of the HbsScanner type.
typedef struct HbsScanner HbsScanner;

// Forward declarations.
typedef struct HbsInputContext HbsInputContext;
typedef struct HbsString HbsString;

// Valid parser tokens.
typedef enum HbsParseTokenType {
    HBS_TOKEN_NULL,         // '\0', or a named constant to signal no token
    HBS_TOKEN_OPEN_BARS,    // "{{"
    HBS_TOKEN_CLOSE_BARS,   // "}}"

    // This token is generated whenever interesting text is discovered. If the
    // ws token is enabled, this token is generated for word input, i.e. [\w]+,
    // but is more akin to .+ whenever the ws token is disabled.
    HBS_TOKEN_TEXT,

    // This token may or may not be generated, based on the value of an
    // internal boolean, which can be enabled or disabled with the *_ws_token
    // functions. See their description for more info.
    HBS_TOKEN_WS,           // [ \t\n]*

    // These tokens, just like the whitespace token, may not be generated
    // unless explicitly enabled by the parser.
    HBS_TOKEN_HASH,         // "#"
    HBS_TOKEN_SLASH,        // "/"

    HBS_TOKEN_EOF, // End of file (or stream)
} HbsParseTokenType;

// Structure of a token event.
typedef struct HbsParseToken {
    HbsParseTokenType type;      // Type of the token

    // The line number and column number in the input that marks the start of
    // the token.
    int line;
    int column;

    // String representing
    HbsString* string;
} HbsParseToken;

// Create a new HbsScanner. The scanner receives input from <input_context>.
HbsScanner* hbs_scanner_new(HbsInputContext* input_context);

// These functions enable or disable the "handlebars" tokens. When we're not
// parsing a handlebars expression, whitespace is signifigcant, so we want it
// to remain intact with the text, but there's also no reason to interrupt the
// parser to notify it of whitespace. The same goes for the octothorpe and
// forward slash symbols. This design choice was aimed at simplifying the logic
// of the scanner at the expense of a slightly more complex interface.
void hbs_scanner_disable_hbs_tokens(HbsScanner* scanner);
void hbs_scanner_enable_hbs_tokens(HbsScanner* scanner);

// Populate <token> with the next token from the stream. Return the number of
// tokens processed (i.e. 1 for a successful scan).
int hbs_scanner_next_symbol(HbsScanner* scanner, HbsParseToken* token);

// Peek at the type of the next token
HbsParseTokenType hbs_scanner_peek(HbsScanner* scanner);

// Free internal memory assocaited with the scanner.
void hbs_scanner_free(HbsScanner* scanner);

// Return a string describing the Parser token type (for debugging purposes)
const char* hbs_token_to_string(HbsParseTokenType type);

// Release internal memory held by <token>.
void hbs_token_release(HbsParseToken* token);

#endif // HANDLEBARS_SCANNER_H

///////////////////////////////////////////////////////////////////////////////
