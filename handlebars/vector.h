///////////////////////////////////////////////////////////////////////////////
// NAME:            vector.h
//
// AUTHOR:          Ethan D. Twardy <ethan.twardy@gmail.com>
//
// DESCRIPTION:     Vector data structure for internal use.
//
// CREATED:         11/25/2021
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

#ifndef HANDLEBARS_VECTOR_H
#define HANDLEBARS_VECTOR_H

#include <stddef.h>

typedef struct HbVector {
    void** vector;
    size_t length;
    size_t capacity;
} HbVector;

HbVector* hb_vector_init();
int hb_vector_push_back(HbVector* vector, void* user_data);
void hb_vector_free(HbVector**, void (*free_data)(void*));

#endif // HANDLEBARS_VECTOR_H

///////////////////////////////////////////////////////////////////////////////
