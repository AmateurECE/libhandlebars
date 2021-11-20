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
// LAST EDITED:     11/20/2021
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

typedef struct HbTemplateContext {
    int unused;
} HbTemplateContext;

typedef struct Handlebars {
    int unused;
} Handlebars;

// TODO: Reference counted templates?
Handlebars* handlebars_template_load_from_file(const char* filename);
char* handlebars_render_template(Handlebars* template,
    HbTemplateContext* context);
void handlebars_template_free(Handlebars** template);

#endif // HANDLEBARS_H

///////////////////////////////////////////////////////////////////////////////
