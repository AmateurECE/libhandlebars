///////////////////////////////////////////////////////////////////////////////
// NAME:            parser.c
//
// AUTHOR:          Ethan D. Twardy <ethan.twardy@gmail.com>
//
// DESCRIPTION:     Implementation of the parser.
//
// CREATED:         12/29/2021
//
// LAST EDITED:     01/04/2022
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

#include <handlebars/handlebars.h>
#include <handlebars/nary-tree.h>
#include <handlebars/parser.h>
#include <handlebars/scanner.h>
#include <handlebars/vector.h>

typedef struct HbParser {
    HbVector* tokens;
    HbScanner* scanner;
    HbNaryNode* tree_top;
} HbParser;

///////////////////////////////////////////////////////////////////////////////
// Private API
////

static void priv_parse_token_free(void* data) {
    HbParseToken* token = (HbParseToken*)data;
    hb_token_release(token);
    free(token);
}

static void priv_component_free(void* data) {
    HbComponent* component = (HbComponent*)data;
    if (NULL != component->text) {
        hb_string_free(&component->text);
    }
    free(data);
}

static int priv_parse_text(HbParser* parser, HbNaryTree* component_tree) {
    HbParseToken* parser_top = hb_vector_pop_back(parser->tokens);
    assert(HB_TOKEN_TEXT == parser_top->type); // Programmer's error.
    HbComponent* component = malloc(sizeof(HbComponent));
    component->type = HB_COMPONENT_TEXT;
    component->text = hb_string_init();
    hb_string_append(component->text, parser_top->string);
    HbNaryNode* node = hb_nary_node_new(component, priv_component_free);
    hb_nary_tree_append_child_to_node(component_tree, parser->tree_top,
        node);
    priv_parse_token_free(parser_top);
    return 0;
}

static int priv_parse_handlebars(HbParser* parser, HbNaryTree* tree) {
    HbParseToken* parser_top = hb_vector_pop_back(parser->tokens);
    assert(HB_TOKEN_CLOSE_BARS == parser_top->type); // Programmer's error.

    HbComponent* component = malloc(sizeof(HbComponent));
    if (NULL == component) {
        return 1;
    }

    component->type = HB_COMPONENT_EXPRESSION;
    component->argv = hb_vector_init();
    while (HB_TOKEN_OPEN_BARS != parser_top->type) {
        hb_token_release(parser_top);
        parser_top = hb_vector_pop_back(parser->tokens);
        if (HB_TOKEN_TEXT == parser_top->type) {
            HbString* argument = hb_string_init();
            hb_string_append(argument, parser_top->string);
            hb_vector_insert(component->argv, 0, argument);
        } else if (HB_TOKEN_OPEN_BARS == parser_top->type) {
            // Do nothing, but especially don't error.
        } else {
            assert(0); // Programmer's error.
        }
    }

    hb_token_release(parser_top);
    HbNaryNode* node = hb_nary_node_new(component, priv_component_free);
    hb_nary_tree_append_child_to_node(tree, parser->tree_top, node);
    return 0;
}

static int priv_rule_handlebars(HbParser* parser, HbNaryTree* component_tree) {
    int result = 1;
    HbParseToken* parser_top = malloc(sizeof(HbParseToken));
    if (NULL == parser_top) {
        return 1;
    }

    hb_vector_push_back(parser->tokens, parser_top);
    hb_scanner_next_symbol(parser->scanner, parser_top);
    if (HB_TOKEN_TEXT == parser_top->type) {
        result = priv_rule_handlebars(parser, component_tree);
    } else if (HB_TOKEN_CLOSE_BARS == parser_top->type) {
        result = priv_parse_handlebars(parser, component_tree);
    } else if (HB_TOKEN_WS == parser_top->type) {
        // Pop this token from the stack and recurse
        HbParseToken* ws_token = hb_vector_pop_back(parser->tokens);
        assert(ws_token == parser_top);
        hb_token_release(parser_top);
        result = priv_rule_handlebars(parser, component_tree);
    } else {
        // TODO: Better error handling here
    }

    return result;
}

static int priv_rule_expression(HbParser* parser, HbNaryTree* component_tree) {
    HbParseToken* parser_top = malloc(sizeof(HbParseToken));
    if (NULL == parser_top) {
        return 1;
    }

    int result = 1;
    hb_vector_push_back(parser->tokens, parser_top);
    hb_scanner_next_symbol(parser->scanner, parser_top);
    if (HB_TOKEN_TEXT == parser_top->type) {
        result = priv_parse_text(parser, component_tree);
    } else if (HB_TOKEN_OPEN_BARS == parser_top->type) {
        hb_scanner_enable_ws_token(parser->scanner);
        result = priv_rule_handlebars(parser, component_tree);
        hb_scanner_disable_ws_token(parser->scanner);
    } else if (HB_TOKEN_EOF == parser_top->type) {
        result = 0; // Do nothing, but especially don't error. EOF is valid.
    } else {
        // TODO: Some kind of error logging here.
        result = 1; // Parse error
    }

    return result;
}

///////////////////////////////////////////////////////////////////////////////
// Public API
////

// Create a new handlebars parser, injecting the scanner. The parser takes
// ownership of and is responsible for freeing the scanner.
HbParser* hb_parser_new(HbScanner* scanner) {
    HbParser* parser = malloc(sizeof(HbParser));
    if (NULL == parser) {
        return NULL;
    }

    parser->scanner = scanner;
    parser->tokens = hb_vector_init();
    if (NULL == parser->tokens) {
        free(parser);
        return NULL;
    }

    return parser;
}

// Free the parser and all associated internal memory.
void hb_parser_free(HbParser* parser) {
    hb_vector_free(&parser->tokens, priv_parse_token_free);
    hb_scanner_free(parser->scanner);
    free(parser);
}

// The parser's job is to construct an Abstract Syntax Tree from the input
// text. The AST is then a direct input to the rendering logic, which performs
// reduce operations in a bottom-up fashion. This function increases the
// reference count on the HbNaryTree which forms the internal representation of
// the AST and returns it. If the parser exited successfully, zero is returned.
// NOTE that the AST returned by this function is NOT the parse tree, but is
// instead a simplified AST optimized for template rendering. Additionally, the
// returned tree is allocated memory, which much be released using
// hb_nary_tree_free().
int hb_parser_parse(HbParser* parser, HbNaryTree** component_tree) {
    int result = 0;
    *component_tree = hb_nary_tree_new();
    if (NULL == *component_tree) {
        return 1;
    }

    HbNaryNode* node = hb_nary_node_new(NULL, NULL);
    if (NULL == node) {
        hb_nary_tree_free(component_tree);
        return 1;
    }
    hb_nary_tree_set_root(*component_tree, node);
    parser->tree_top = node;

    HbParseToken* top = NULL;
    do {
        int parse_result = priv_rule_expression(parser, *component_tree);
        if (0 != parse_result) {
            hb_nary_tree_free(component_tree);
            return 1;
        }

        if (0 < parser->tokens->length) {
            // If the first call parsed an entire expression, but didn't reach
            // end of stream, the parser stack will be empty, and this will be
            // a bad access.
            top = parser->tokens->vector[parser->tokens->length - 1];
        }
    } while (NULL == top || HB_TOKEN_EOF != top->type);

    return result;
}

///////////////////////////////////////////////////////////////////////////////
