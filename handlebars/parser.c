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

void priv_parse_token_free(void* data) {
    HbParseToken* token = (HbParseToken*)data;
    hb_token_release(token);
    free(token);
}

void priv_component_free(void* data) {
    HbComponent* component = (HbComponent*)data;
    if (NULL != component->text) {
        hb_string_free(&component->text);
    }
    free(data);
}

int priv_rule_expression(HbParser* parser, HbNaryTree* component_tree) {
    HbParseToken* parser_top = malloc(sizeof(HbParseToken));
    if (NULL == parser_top) {
        return 0;
    }

    int result = 0;
    hb_vector_push_back(parser->tokens, parser_top);
    hb_scanner_next_symbol(parser->scanner, parser_top);
    if (HB_TOKEN_TEXT == parser_top->type) {
        HbComponent* component = malloc(sizeof(HbComponent));
        component->type = HB_COMPONENT_TEXT;
        component->text = hb_string_init();
        hb_string_append(component->text, parser_top->string);
        HbNaryNode* node = hb_nary_node_new(component, priv_component_free);
        hb_nary_tree_append_child_to_node(component_tree, parser->tree_top,
            node);
        assert(parser_top == hb_vector_pop_back(parser->tokens));
        hb_token_release(parser_top);
        result = 1;
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

    int parse_result = priv_rule_expression(parser, *component_tree);
    HbParseToken* top = parser->tokens->vector[parser->tokens->length - 1];
    while (0 < parse_result || HB_TOKEN_EOF != top->type) {
        parse_result = priv_rule_expression(parser, *component_tree);
        top = parser->tokens->vector[parser->tokens->length - 1];
    }

    return result;
}

///////////////////////////////////////////////////////////////////////////////
