///////////////////////////////////////////////////////////////////////////////
// NAME:            parser.c
//
// AUTHOR:          Ethan D. Twardy <ethan.twardy@gmail.com>
//
// DESCRIPTION:     Implementation of the parser.
//
// CREATED:         12/29/2021
//
// LAST EDITED:     01/06/2022
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

typedef struct HbsParser {
    HbsVector* tokens;
    HbsScanner* scanner;
    HbsNaryNode* tree_top;
} HbsParser;

///////////////////////////////////////////////////////////////////////////////
// Private API
////

static void priv_parse_token_free(void* data) {
    HbsParseToken* token = (HbsParseToken*)data;
    hbs_token_release(token);
    free(token);
}

static void priv_component_free(void* data) {
    HbsComponent* component = (HbsComponent*)data;
    if (NULL != component->text) {
        hbs_string_free(component->text);
    }
    free(data);
}

static int priv_parse_text(HbsParser* parser, HbsNaryTree* component_tree) {
    HbsParseToken* parser_top = hbs_vector_pop_back(parser->tokens);
    assert(HBS_TOKEN_TEXT == parser_top->type); // Programmer's error.
    HbsComponent* component = malloc(sizeof(HbsComponent));
    component->type = HBS_COMPONENT_TEXT;
    component->text = hbs_string_init();
    hbs_string_append(component->text, parser_top->string);
    HbsNaryNode* node = hbs_nary_node_new(component, priv_component_free);
    hbs_nary_tree_append_child_to_node(component_tree, parser->tree_top,
        node);
    priv_parse_token_free(parser_top);
    return 0;
}

static int priv_parse_handlebars(HbsParser* parser, HbsNaryTree* tree) {
    HbsParseToken* parser_top = hbs_vector_pop_back(parser->tokens);
    assert(HBS_TOKEN_CLOSE_BARS == parser_top->type); // Programmer's error.

    HbsComponent* component = malloc(sizeof(HbsComponent));
    if (NULL == component) {
        return 1;
    }

    int result = 0;
    component->type = HBS_COMPONENT_EXPRESSION;
    component->argv = hbs_vector_init();
    while (HBS_TOKEN_OPEN_BARS != parser_top->type) {
        hbs_token_release(parser_top);
        parser_top = hbs_vector_pop_back(parser->tokens);
        if (HBS_TOKEN_TEXT == parser_top->type) {
            HbsString* argument = hbs_string_init();
            hbs_string_append(argument, parser_top->string);
            hbs_vector_insert(component->argv, 0, argument);
        } else if (HBS_TOKEN_OPEN_BARS == parser_top->type) {
            // As long as there was more than one text token between the
            // open token and close token, this is a valid expression.
            if (0 == component->argv->length) {
                result = 1;
            }
        } else {
            assert(0); // Programmer's error.
        }
    }

    hbs_token_release(parser_top);
    HbsNaryNode* node = hbs_nary_node_new(component, priv_component_free);
    hbs_nary_tree_append_child_to_node(tree, parser->tree_top, node);
    return result;
}

static int priv_rule_handlebars(HbsParser* parser, HbsNaryTree* component_tree) {
    int result = 1;
    HbsParseToken* parser_top = malloc(sizeof(HbsParseToken));
    if (NULL == parser_top) {
        return 1;
    }

    hbs_vector_push_back(parser->tokens, parser_top);
    hbs_scanner_next_symbol(parser->scanner, parser_top);
    if (HBS_TOKEN_TEXT == parser_top->type) {
        result = priv_rule_handlebars(parser, component_tree);
    } else if (HBS_TOKEN_CLOSE_BARS == parser_top->type) {
        result = priv_parse_handlebars(parser, component_tree);
    } else if (HBS_TOKEN_WS == parser_top->type) {
        // Pop this token from the stack and recurse
        HbsParseToken* ws_token = hbs_vector_pop_back(parser->tokens);
        assert(ws_token == parser_top);
        hbs_token_release(parser_top);
        result = priv_rule_handlebars(parser, component_tree);
    } else {
        // TODO: Better error handling here
    }

    return result;
}

static int priv_rule_expression(HbsParser* parser, HbsNaryTree* component_tree)
{
    HbsParseToken* parser_top = malloc(sizeof(HbsParseToken));
    if (NULL == parser_top) {
        return 1;
    }

    int result = 1;
    hbs_vector_push_back(parser->tokens, parser_top);
    hbs_scanner_next_symbol(parser->scanner, parser_top);
    if (HBS_TOKEN_TEXT == parser_top->type) {
        result = priv_parse_text(parser, component_tree);
    } else if (HBS_TOKEN_OPEN_BARS == parser_top->type) {
        hbs_scanner_enable_ws_token(parser->scanner);
        result = priv_rule_handlebars(parser, component_tree);
        hbs_scanner_disable_ws_token(parser->scanner);
    } else if (HBS_TOKEN_EOF == parser_top->type) {
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
HbsParser* hbs_parser_new(HbsScanner* scanner) {
    HbsParser* parser = malloc(sizeof(HbsParser));
    if (NULL == parser) {
        return NULL;
    }

    parser->scanner = scanner;
    parser->tokens = hbs_vector_init();
    if (NULL == parser->tokens) {
        free(parser);
        return NULL;
    }

    return parser;
}

// Free the parser and all associated internal memory.
void hbs_parser_free(HbsParser* parser) {
    hbs_vector_free(parser->tokens, priv_parse_token_free);
    hbs_scanner_free(parser->scanner);
    free(parser);
}

// The parser's job is to construct an Abstract Syntax Tree from the input
// text. The AST is then a direct input to the rendering logic, which performs
// reduce operations in a bottom-up fashion. This function increases the
// reference count on the HbsNaryTree which forms the internal representation
// of the AST and returns it. If the parser exited successfully, zero is
// returned.
// NOTE that the AST returned by this function is NOT the parse tree, but is
// instead a simplified AST optimized for template rendering. Additionally, the
// returned tree is allocated memory, which much be released using
// hbs_nary_tree_free().
int hbs_parser_parse(HbsParser* parser, HbsNaryTree** component_tree) {
    int result = 0;
    *component_tree = hbs_nary_tree_new();
    if (NULL == *component_tree) {
        return 1;
    }

    HbsNaryNode* node = hbs_nary_node_new(NULL, NULL);
    if (NULL == node) {
        hbs_nary_tree_free(*component_tree);
        *component_tree = NULL;
        return 1;
    }
    hbs_nary_tree_set_root(*component_tree, node);
    parser->tree_top = node;

    HbsParseToken* top = NULL;
    do {
        int parse_result = priv_rule_expression(parser, *component_tree);
        if (0 != parse_result) {
            hbs_nary_tree_free(*component_tree);
            *component_tree = NULL;
            return 1;
        }

        if (0 < parser->tokens->length) {
            // If the first call parsed an entire expression, but didn't reach
            // end of stream, the parser stack will be empty, and this will be
            // a bad access.
            top = parser->tokens->vector[parser->tokens->length - 1];
        }
    } while (NULL == top || HBS_TOKEN_EOF != top->type);

    return result;
}

///////////////////////////////////////////////////////////////////////////////
