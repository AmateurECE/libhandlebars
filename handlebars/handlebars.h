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

#ifndef HANDLEBARS_H
#define HANDLEBARS_H

#include <stddef.h>
#include <stdlib.h>

// Generic object for getting input to the parser. This struct can be allocated
// on the stack, created by the user, or created using one of the convenience
// functions provided.
typedef struct HbInputContext {
    size_t (*read)(void* data, char* buffer, size_t buffer_size);
    void (*free_data)(void* data);
    void* data;
} HbInputContext;

// Handlers can return these to indicate success or stop rendering
typedef enum HbResult {
    HB_OK,
    HB_ERROR,
} HbResult;

// SAX-style interface for callbacks to render expressions.
typedef struct HbHandlers {
    // Key handler. For a plain ol' context substitution expression, for
    // example, "{{somedata}}", <key> would be "somedata", and the handler
    // would set "value" equal to whatever value we want substituted here, e.g.
    // "foo".  <key_handler_data> is the value of the struct member
    // key_handler_data.
    HbResult (*key_handler)(void* key_handler_data, const char* key,
        const char** value);
    void* key_handler_data;
} HbHandlers;

// Opaque struct representing a loaded Handlebars template.
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

// Load the template from the input context. After this, the input context
// can be freed (if necessary).
Handlebars* handlebars_template_load(HbInputContext* input_context);

// Render the template. <handlers> is used to obtain data ("context") for
// rendering the template. The output is an HbString object which must be
// free'd using hb_string_free() after use to prevent memory leaks.
HbString* handlebars_template_render(Handlebars* template,
    HbHandlers* handlers);

// Free the template
void handlebars_template_free(Handlebars** template);

#endif // HANDLEBARS_H

///////////////////////////////////////////////////////////////////////////////
