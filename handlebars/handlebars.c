///////////////////////////////////////////////////////////////////////////////
// NAME:            handlebars.c
//
// AUTHOR:          Ethan D. Twardy <ethan.twardy@gmail.com>
//
// DESCRIPTION:     Parser entrypoint
//
// CREATED:         11/20/2021
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
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

#include <handlebars/handlebars.h>
#include <handlebars/nary-tree.h>
#include <handlebars/parser.h>
#include <handlebars/scanner.h>
#include <handlebars/vector.h>

// This struct contains context necessary to parse the template and render it
// using context.
typedef struct Handlebars {
    HbNaryTree* components;
} Handlebars;

///////////////////////////////////////////////////////////////////////////////
// Private API
////

int priv_render_substitution(HbComponent* component, HbString* string,
    HbHandlers* handlers)
{
    assert(NULL != handlers->key_handler);
    const char* value = NULL;
    HbString* key = (HbString*)component->argv->vector[0];

    int result = handlers->key_handler(handlers->key_handler_data,
        key->string, &value);
    if (0 != result) {
        return 1;
    }

    if (0 != hb_string_append_str(string, value)) {
        return 1;
    }
    return 0;
}

int priv_render_component(HbComponent* component, HbString* result,
    HbHandlers* handlers)
{
    switch (component->type) {
    case HB_COMPONENT_TEXT:
        return hb_string_append(result, component->text);

    case HB_COMPONENT_EXPRESSION: {
        if (1 == component->argv->length) {
            return priv_render_substitution(component, result, handlers);
        } else {
            return 1;
        }
    }

    default:
        return HB_ERROR;
    }
}

///////////////////////////////////////////////////////////////////////////////
// Public API
////

// Load a template in from the input_context. If the input_context is backed by
// a file, this function will construct the template components from the
// contents of that file as it is on disk currently. The template is not
// reloaded every time the template is rendered (unless explicitly done so).
Handlebars* handlebars_template_load(HbInputContext* input_context) {
    Handlebars* template = malloc(sizeof(Handlebars));
    if (NULL == template) {
        return NULL;
    }

    HbScanner* scanner = hb_scanner_new(input_context);
    if (NULL == scanner) {
        free(template);
        return NULL;
    }
    HbParser* parser = hb_parser_new(scanner);
    if (NULL == parser) {
        hb_scanner_free(scanner);
        free(template);
        return NULL;
    }

    memset(template, 0, sizeof(Handlebars));
    int parse_result = hb_parser_parse(parser, &template->components);
    if (0 != parse_result) {
        hb_parser_free(parser);
        free(template);
        return NULL;
    }

    return template;
}

// Render the template using the template context. The input context contains
// all context data and helpers (with the exception of the default helpers). If
// the template contains expressions which don't match up to entries in the
// context, those expressions are rendered as the empty string "".
HbString* handlebars_template_render(Handlebars* template,
    HbHandlers* handlers)
{
    HbString* result = hb_string_init();

    HbNaryTreeIter iterator;
    hb_nary_tree_iter_init(&iterator, template->components);
    HbNaryNode* element = NULL;
    HbNaryNode* root = hb_nary_tree_get_root(template->components);

    while (root != (element = hb_nary_tree_iter_next(&iterator))) {
        HbComponent* component = hb_nary_node_get_data(element);
        if (0 != priv_render_component(component, result, handlers)) {
            hb_string_free(&result);
            return NULL;
        }
    }

    return result;
}

// Free the template components, relinquishing all allocated memory back to the
// system.
void handlebars_template_free(Handlebars** template) {
    if (NULL != *template) {
        if (NULL != (*template)->components) {
            hb_nary_tree_free(&(*template)->components);
        }

        free(*template);
        *template = NULL;
    }
}

///////////////////////////////////////////////////////////////////////////////
