///////////////////////////////////////////////////////////////////////////////
// NAME:            template-context.c
//
// AUTHOR:          Ethan D. Twardy <ethan.twardy@gmail.com>
//
// DESCRIPTION:     Implementation of the HbTemplateContext API
//
// CREATED:         11/24/2021
//
// LAST EDITED:     11/25/2021
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

#include <handlebars/handlebars.h>
#include <handlebars/string.h>
#include <handlebars/vector.h>

///////////////////////////////////////////////////////////////////////////////
// Private API
////

typedef struct HbContextEntry {
    HbString* key;
    HbString* value;
} HbContextEntry;

static void hb_priv_context_entry_free(void* element) {
    HbContextEntry* entry = (HbContextEntry*)element;
    hb_string_free(&entry->key);
    hb_string_free(&entry->value);
    free(entry);
}

///////////////////////////////////////////////////////////////////////////////
// Public API
////

HbTemplateContext* handlebars_template_context_init() {
    HbTemplateContext* context = malloc(sizeof(HbTemplateContext));
    if (NULL == context) {
        return NULL;
    }

    context->context = hb_vector_init();
    if (NULL == context->context) {
        free(context);
        return NULL;
    }

    return context;
}

void handlebars_template_context_free(HbTemplateContext** context) {
    if (NULL == *context) {
        return;
    }

    hb_vector_free(&(*context)->context, hb_priv_context_entry_free);
    free(*context);
    *context = NULL;
}

// Set a string in the context
int handlebars_template_context_set_string(HbTemplateContext* context,
    const char* key, const char* value)
{
    HbContextEntry* entry = malloc(sizeof(HbContextEntry));
    if (NULL == entry) {
        return 1;
    }

    HbString* string_key = hb_string_from_str(key);
    if (NULL == string_key) {
        free(entry);
        return 1;
    }

    HbString* string_value = hb_string_from_str(value);
    if (NULL == string_key) {
        free(entry);
        hb_string_free(&string_key);
        return 1;
    }

    entry->key = string_key;
    entry->value = string_value;
    if (0 != hb_vector_push_back(context->context, entry)) {
        free(entry);
        hb_string_free(&string_key);
        hb_string_free(&string_value);
        return 1;
    }

    return 0;
}

///////////////////////////////////////////////////////////////////////////////
