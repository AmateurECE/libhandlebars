///////////////////////////////////////////////////////////////////////////////
// NAME:            linked-list.c
//
// AUTHOR:          Ethan D. Twardy <ethan.twardy@gmail.com>
//
// DESCRIPTION:     Implementation of the linked list.
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

#include <handlebars/linked-list.h>

///////////////////////////////////////////////////////////////////////////////
// Public API
////

void hb_list_init(HbList* list) {
    list->head = NULL;
    list->tail = NULL;
}

HbCons* hb_list_append(HbList* list, void* user_data)
{
    HbCons* cons = malloc(sizeof(HbCons));
    if (NULL == cons) {
        return NULL;
    }

    cons->next = NULL;
    cons->data = user_data;
    list->tail = cons;
    if (NULL == list->head) {
        list->head = cons;
    }

    list->length += 1;
    return cons;
}

void* hb_list_pop_front(HbList* list) {
    if (NULL == list->head) {
        return NULL;
    }

    HbCons* element = list->head;
}

///////////////////////////////////////////////////////////////////////////////
