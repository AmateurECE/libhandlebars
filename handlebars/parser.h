///////////////////////////////////////////////////////////////////////////////
// NAME:            parser.h
//
// AUTHOR:          Ethan D. Twardy <ethan.twardy@gmail.com>
//
// DESCRIPTION:     Interface for an LALR(1) parser to parse Handlebars
//                  templates.
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

#ifndef HANDLEBARS_PARSER_H
#define HANDLEBARS_PARSER_H

// Opaque typedef for the parser.
typedef struct HbParser HbParser;

// Forward declarations
typedef struct HbScanner HbScanner;
typedef struct HbNaryTree HbNaryTree;

// Create a new handlebars parser, injecting the scanner. The parser takes
// ownership of and is responsible for freeing the scanner.
HbParser* hb_parser_new(HbScanner* scanner, void* user_data);

// Free the parser and all associated internal memory.
void hb_parser_free(HbParser* parser);

// Iterate the parser. If the parser has reached a syntax error, acceptance,
// end of stream, or other terminating condition, zero is returned.
int hb_parser_parse(HbParser* parser);

// The parser's job is to construct an Abstract Syntax Tree from the input
// text. The AST is then a direct input to the rendering logic, which performs
// reduce operations in a bottom-up fashion. This function increases the
// reference count on the HbNaryTree which forms the internal representation of
// the AST and returns it. If the parser has not terminated, this function
// returns NULL. NOTE that the AST returned by this function is NOT the parse
// tree, but is instead a simplified AST optimized for template rendering.
HbNaryTree* hb_parser_get_ast(HbParser* parser);

#endif // HANDLEBARS_PARSER_H

///////////////////////////////////////////////////////////////////////////////
