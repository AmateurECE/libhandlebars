///////////////////////////////////////////////////////////////////////////////
// NAME:            linked-list.h
//
// AUTHOR:          Ethan D. Twardy <ethan.twardy@gmail.com>
//
// DESCRIPTION:     Linked List to support parser.
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

#ifndef HANDLEBARS_LINKED_LIST_H
#define HANDLEBARS_LINKED_LIST_H

typedef struct HbCons HbCons;
typedef struct HbCons {
    void* data;
    HbCons* next;
} HbCons;

typedef struct HbList {
    HbCons* head;
    HbCons* tail;
    size_t length;
} HbList;

void hb_list_init(HbList* list);
HbCons* hb_list_push_back(HbList* list, void* user_data);
void* hb_list_pop_front(HbList* list);

#endif // HANDLEBARS_LINKED_LIST_H

///////////////////////////////////////////////////////////////////////////////
