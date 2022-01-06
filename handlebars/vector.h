///////////////////////////////////////////////////////////////////////////////
// NAME:            vector.h
//
// AUTHOR:          Ethan D. Twardy <ethan.twardy@gmail.com>
//
// DESCRIPTION:     Vector data structure for internal use.
//
// CREATED:         11/25/2021
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

#ifndef HANDLEBARS_VECTOR_H
#define HANDLEBARS_VECTOR_H

#include <stddef.h>

typedef struct HbsVector {
    void** vector;
    size_t length;
    size_t capacity;
} HbsVector;

typedef void VectorFreeDataFn(void*);

HbsVector* hbs_vector_new();
int hbs_vector_push_back(HbsVector* vector, void* user_data);
void* hbs_vector_pop_back(HbsVector* vector);
int hbs_vector_insert(HbsVector* vector, size_t index, void* user_data);
void hbs_vector_free(HbsVector*, VectorFreeDataFn* free_data);

#endif // HANDLEBARS_VECTOR_H

///////////////////////////////////////////////////////////////////////////////
