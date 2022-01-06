///////////////////////////////////////////////////////////////////////////////
// NAME:            string.c
//
// AUTHOR:          Ethan D. Twardy <ethan.twardy@gmail.com>
//
// DESCRIPTION:     Implementation of the String methods.
//
// CREATED:         11/22/2021
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

#include <stdlib.h>
#include <string.h>

#include <handlebars/handlebars.h>

// Attempt to make it easy on myself by making the default size the size of a
// cache line?
static const size_t DEFAULT_CAPACITY = 64;

///////////////////////////////////////////////////////////////////////////////
// Private API
////

static int hbs_priv_string_extend(HbsString* first, size_t needed_capacity) {
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

HbsString* hbs_string_init() {
    HbsString* string = malloc(sizeof(HbsString));
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
    memset(string->string, 0, string->capacity);
    return string;
}

HbsString* hbs_string_from_str(const char* content) {
    HbsString* string = hbs_string_init();
    if (NULL == string) {
        return NULL;
    }

    if (0 != hbs_string_append_str(string, content)) {
        free(string);
        return NULL;
    }
    return string;
}

int hbs_string_append(HbsString* first, const HbsString* second) {
    return hbs_string_append_str(first, second->string);
}

int hbs_string_append_str(HbsString* first, const char* second)
{
    const size_t append_length = strlen(second);
    const size_t needed_capacity = append_length + first->length + 1;
    if (needed_capacity > first->capacity) {
        if (0 != hbs_priv_string_extend(first, needed_capacity)) {
            return 1;
        }
    }

    strncpy(first->string + first->length, second, append_length + 1);
    first->length = append_length + first->length;
    first->string[first->length] = '\0';
    return 0;
}

void hbs_string_free(HbsString** string) {
    if (NULL != *string) {
        free((*string)->string);
        free(*string);
        *string = NULL;
    }
}

///////////////////////////////////////////////////////////////////////////////
