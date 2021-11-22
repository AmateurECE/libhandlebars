///////////////////////////////////////////////////////////////////////////////
// NAME:            handlebars.c
//
// AUTHOR:          Ethan D. Twardy <ethan.twardy@gmail.com>
//
// DESCRIPTION:     Parser entrypoint
//
// CREATED:         11/20/2021
//
// LAST EDITED:     11/22/2021
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
#include <handlebars/state-machine.h>

typedef struct _yycontext yycontext;
static void hb_priv_input(yycontext* context, char* buffer, int* result,
    int max_size);
#define YY_CTX_LOCAL 1
#define YY_INPUT(context, buffer, result, max_size)     \
    hb_priv_input(context, buffer, &result, max_size);
#define YY_CTX_MEMBERS Handlebars* handlebars;
#include "parser.c"

///////////////////////////////////////////////////////////////////////////////
// Private API
////

// Invoke the HbInputContext to fill the parser buffer
void hb_priv_input(yycontext* context, char* buffer, int* result,
    int max_size)
{
    Handlebars* handlebars = context->handlebars;
    HbInputContext* input_context = handlebars->input_context;
    assert(NULL != input_context);

    int bytes_copied = input_context->read(input_context->data, buffer,
        max_size);
    *result = bytes_copied;
}

///////////////////////////////////////////////////////////////////////////////
// Public API
////

Handlebars* handlebars_template_load(HbInputContext* input_context) {
    Handlebars* template = malloc(sizeof(Handlebars));
    if (NULL == template) {
        return NULL;
    }

    template->input_context = input_context;
    yycontext context;
    memset(&context, 0, sizeof(yycontext));
    context.handlebars = template;
    hb_list_init(&template->parser_events);
    hb_event(template, SIGNAL_DOCUMENT_START);

    MealyFsm* parser_machine = hb_parser_machine_init(template);
    while (yyparse(&context))
        hb_parser_machine_iterate(parser_machine);

    hb_event(template, SIGNAL_DOCUMENT_END);
    hb_parser_machine_iterate(parser_machine);
    hb_parser_machine_free(&parser_machine);
    hb_list_free(&template->parser_events);

    template->input_context = NULL;
    return template;
}

char* handlebars_render_template(Handlebars* template,
    HbTemplateContext* context)
{ return NULL; }

void handlebars_template_free(Handlebars** template) {
    if (NULL != *template) {
        free(*template);
        *template = NULL;
    }
}

///////////////////////////////////////////////////////////////////////////////
