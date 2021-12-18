///////////////////////////////////////////////////////////////////////////////
// NAME:            handlebars.h
//
// AUTHOR:          Ethan D. Twardy <ethan.twardy@gmail.com>
//
// DESCRIPTION:     libhandlebars is a library for parsing and rendering
//                  handlebars templates.
//
// CREATED:         11/20/2021
//
// LAST EDITED:     12/17/2021
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

#ifndef HANDLEBARS_H
#define HANDLEBARS_H

#include <stddef.h>
#include <stdlib.h>

typedef struct HbVector HbVector;

// Generic object for getting input to the parser. This struct can be allocated
// on the stack, created by the user, or created using one of the convenience
// functions provided.
typedef struct HbInputContext {
    size_t (*read)(void* data, char* buffer, size_t buffer_size);
    void (*free_data)(void* data);
    void* data;
} HbInputContext;

typedef struct HbTemplateContext {
    HbVector* context;
} HbTemplateContext;

// Don't try to modify any of the members of this struct.
typedef struct Handlebars Handlebars;

typedef struct HbString {
    char* string;
    size_t length;
    size_t capacity;
} HbString;

// Initialize a string
HbString* hb_string_init();

// Create a string from a C-style string (requires a copy)
HbString* hb_string_from_str(const char* string);

// Append two strings
int hb_string_append(HbString* first, const HbString* second);
int hb_string_append_str(HbString* first, const char* second);

// Free all memory associated with a string
void hb_string_free(HbString** string);

// Create an input context from the file with the given path
HbInputContext* handlebars_input_context_from_file(const char* filename);

// Create an input context from a string
HbInputContext* handlebars_input_context_from_string(const char* string);

// Free the input context (only necessary for HbInputContext instances created
// using library convenience functions)
void handlebars_input_context_free(HbInputContext** input_context);

// TODO: Reference counted templates?

// Load the template from the input context
Handlebars* handlebars_template_load(HbInputContext* input_context);

// Render the template with the given context
HbString* handlebars_template_render(Handlebars* template,
    HbTemplateContext* context);

// Free the template
void handlebars_template_free(Handlebars** template);

HbTemplateContext* handlebars_template_context_init();
void handlebars_template_context_free(HbTemplateContext** context);

// Set a string in the context
int handlebars_template_context_set_string(HbTemplateContext* context,
    const char* key, const char* value);

// Get a string from the context
const HbString* handlebars_template_context_get(HbTemplateContext* context,
    HbString* key);
// TODO: handlebars_template_context_set_object
// TODO: handlebars_template_context_set_int

#endif // HANDLEBARS_H

///////////////////////////////////////////////////////////////////////////////
