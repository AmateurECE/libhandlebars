///////////////////////////////////////////////////////////////////////////////
// NAME:            string.c
//
// AUTHOR:          Ethan D. Twardy <ethan.twardy@gmail.com>
//
// DESCRIPTION:     Implementation of the String methods.
//
// CREATED:         11/22/2021
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

#include <stdlib.h>
#include <string.h>

#include <handlebars/string.h>

// Should probably get this with sysconf, but...eh.
static const size_t DEFAULT_CAPACITY = 4096;

///////////////////////////////////////////////////////////////////////////////
// Private API
////

static int hb_priv_string_extend(HbString* first, size_t needed_capacity) {
    size_t new_capacity = first->capacity;
    while (new_capacity < needed_capacity)
        new_capacity *= 2;
    void* temp_string = realloc(first->string, new_capacity);
    if (temp_string == first->string) {
        first->capacity = new_capacity;
        return 0;
    } else if (temp_string == NULL) {
        return 1;
    } else {
        first->string = temp_string;
        first->capacity = new_capacity;
        return 0;
    }
}

///////////////////////////////////////////////////////////////////////////////
// Public API
////

HbString* hb_string_init() {
    HbString* string = malloc(sizeof(HbString));
    if (NULL == string) {
        return NULL;
    }

    string->capacity = DEFAULT_CAPACITY;
    string->string = malloc(DEFAULT_CAPACITY);
    if (NULL == string->string) {
        free(string);
        return NULL;
    }
    string->length = 0;
    return string;
}

int hb_string_append_str(HbString* first, const char* second)
{
    const size_t append_length = strlen(second);
    const size_t needed_capacity = append_length + first->length + 1;
    if (needed_capacity > first->capacity) {
        if (0 != hb_priv_string_extend(first, needed_capacity)) {
            return 1;
        }
    }

    strncpy(first->string, second, append_length + 1);
    first->length = append_length + first->length;
    first->string[first->length] = '\0';
    return 0;
}

///////////////////////////////////////////////////////////////////////////////
