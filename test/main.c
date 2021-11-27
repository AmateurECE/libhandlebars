///////////////////////////////////////////////////////////////////////////////
// NAME:            main.c
//
// AUTHOR:          Ethan D. Twardy <ethan.twardy@gmail.com>
//
// DESCRIPTION:     Entrypoint for test application.
//
// CREATED:         11/20/2021
//
// LAST EDITED:     11/27/2021
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

#include <stdio.h>

#include <handlebars.h>

static const char* template_text = "Some {{ test }}\n";

int main() {
    HbInputContext* input_context =
        handlebars_input_context_from_string(template_text);
    Handlebars* template = handlebars_template_load(input_context);
    HbTemplateContext* template_context = handlebars_template_context_init();
    handlebars_template_context_set_string(template_context, "test", "thing");
    HbString* output = handlebars_template_render(template, template_context);
    if (NULL == output) {
        return 1;
    }

    printf("%s", output->string);
    hb_string_free(&output);
    handlebars_template_context_free(&template_context);
    handlebars_input_context_free(&input_context);
    handlebars_template_free(&template);
}

///////////////////////////////////////////////////////////////////////////////
