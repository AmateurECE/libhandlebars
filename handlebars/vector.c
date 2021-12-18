///////////////////////////////////////////////////////////////////////////////
// NAME:            vector.c
//
// AUTHOR:          Ethan D. Twardy <ethan.twardy@gmail.com>
//
// DESCRIPTION:     Implementation of the vector.
//
// CREATED:         11/25/2021
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

#include <stdlib.h>
#include <string.h>

#include <handlebars/vector.h>

// Attempt to make it easy on myself by making the default size the size of a
// cache line?
static const size_t DEFAULT_CAPACITY = 8;

///////////////////////////////////////////////////////////////////////////////
// Private API
////

static int hb_priv_vector_extend(HbVector* first, size_t needed_capacity) {
    size_t new_capacity = first->capacity;
    while (new_capacity < needed_capacity)
        new_capacity *= 2;
    void* temp_vector = realloc(first->vector, new_capacity);
    if (temp_vector == first->vector) {
        first->capacity = new_capacity;
        return 0;
    } else if (temp_vector == NULL) {
        return 1;
    } else {
        first->vector = temp_vector;
        first->capacity = new_capacity;
        return 0;
    }
}

///////////////////////////////////////////////////////////////////////////////
// Public API
////

HbVector* hb_vector_init() {
    HbVector* vector = malloc(sizeof(HbVector));
    if (NULL == vector) {
        return NULL;
    }

    vector->capacity = DEFAULT_CAPACITY;
    vector->vector = calloc(sizeof(void*), DEFAULT_CAPACITY);
    if (NULL == vector->vector) {
        free(vector);
        return NULL;
    }
    memset(vector->vector, 0, sizeof(void*) * DEFAULT_CAPACITY);
    vector->length = 0;
    return vector;
}

int hb_vector_push_back(HbVector* vector, void* user_data) {
    if (vector->length >= vector->capacity && hb_priv_vector_extend(vector,
            vector->length + 1)) {
        return 1;
    }

    vector->vector[vector->length++] = user_data;
    return 0;
}

int hb_vector_insert(HbVector* vector, size_t index, void* user_data) {
    if (index > vector->length) {
        return 1;
    }

    if (vector->length >= vector->capacity && hb_priv_vector_extend(vector,
            vector->length + 1)) {
        return 2;
    }

    for (size_t i = vector->length; i > index; --i) {
        vector->vector[i] = vector->vector[i - 1];
    }

    vector->vector[index] = user_data;
    ++vector->length;
    return 0;
}

void hb_vector_free(HbVector** vector, void (*free_data)(void*)) {
    if (NULL == *vector) {
        return;
    }

    if (NULL != free_data) {
        for (size_t i = 0; i < (*vector)->length; ++i) {
            free_data((*vector)->vector[i]);
        }
    }

    free((*vector)->vector);
    free(*vector);
    *vector = NULL;
}

///////////////////////////////////////////////////////////////////////////////
