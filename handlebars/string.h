///////////////////////////////////////////////////////////////////////////////
// NAME:            string.h
//
// AUTHOR:          Ethan D. Twardy <ethan.twardy@gmail.com>
//
// DESCRIPTION:     Dynamic string class to support parsing
//
// CREATED:         11/22/2021
//
// LAST EDITED:     11/23/2021
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

#ifndef HANDLEBARS_STRING_H
#define HANDLEBARS_STRING_H

typedef struct HbString {
    char* string;
    size_t length;
    size_t capacity;
} HbString;

HbString* hb_string_init();
HbString* hb_string_copy_from_str(const char* string);
int hb_string_append(HbString* first, HbString* second);
int hb_string_append_str(HbString* first, const char* second);
void hb_string_free(HbString** string);

#endif // HANDLEBARS_STRING_H

///////////////////////////////////////////////////////////////////////////////
