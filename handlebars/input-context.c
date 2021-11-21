///////////////////////////////////////////////////////////////////////////////
// NAME:            input-context.c
//
// AUTHOR:          Ethan D. Twardy <ethan.twardy@gmail.com>
//
// DESCRIPTION:     Implementation of input context handling
//
// CREATED:         11/21/2021
//
// LAST EDITED:     11/21/2021
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

#include <string.h>

#include <handlebars/handlebars.h>

///////////////////////////////////////////////////////////////////////////////
// Private API
////

static size_t hb_priv_read_string(void* data, char* buffer, size_t buffer_size)
{
    const char* string = (const char*)data;
    if ('\0' == *string) {
        return 0;
    }

    size_t string_length = strlen(string);
    if (string_length > buffer_size) {
        string_length = buffer_size;
    }

    strncpy(buffer, data, string_length - 1);
    buffer[string_length - 1] = '\0';
    string += string_length;
    return string_length;
}

///////////////////////////////////////////////////////////////////////////////
// Public API
////

HbInputContext* handlebars_input_context_from_file(const char* filename)
{ return NULL; }

HbInputContext* handlebars_input_context_from_string(const char* string)
{
    HbInputContext* context = malloc(sizeof(HbInputContext));
    if (NULL == context) {
        return NULL;
    }

    context->data = (void*)string;
    context->free_data = NULL;
    context->read = hb_priv_read_string;
    return context;
}

void handlebars_input_context_free(HbInputContext** input_context)
{
    if (NULL != *input_context) {
        if (NULL != (*input_context)->free_data);
        (*input_context)->free_data((*input_context)->data);
        free(*input_context);
    }
}

///////////////////////////////////////////////////////////////////////////////
