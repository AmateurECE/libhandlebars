///////////////////////////////////////////////////////////////////////////////
// NAME:            state-machine.h
//
// AUTHOR:          Ethan D. Twardy <ethan.twardy@gmail.com>
//
// DESCRIPTION:     The parser state machine, for converting the token stream
//                  into a renderable template
//
// CREATED:         11/21/2021
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

#ifndef HANDLEBARS_STATE_MACHINE_H
#define HANDLEBARS_STATE_MACHINE_H

typedef struct Handlebars Handlebars;
typedef struct MealyFsm MealyFsm;

enum States {
    STATE_INVALID,
    STATE_TEXT,
    STATE_TEMPLATE,
    STATE_FINAL,
};

enum Event {
    SIGNAL_NONE,
    SIGNAL_DOCUMENT_START,
    SIGNAL_DOCUMENT_END,
    SIGNAL_TEXT,
    SIGNAL_START_TOKEN,
    SIGNAL_END_TOKEN,
};

void hb_event(Handlebars* handlebars, enum Event event);
void hb_string(Handlebars* handlebars, const char* string);
int hb_read_event(int* state, void* user_data);

MealyFsm* hb_parser_machine_init(Handlebars* handlebars);
int hb_parser_machine_iterate(MealyFsm* machine);
void hb_parser_machine_free(MealyFsm** machine);

#endif // HANDLEBARS_STATE_MACHINE_H

///////////////////////////////////////////////////////////////////////////////
