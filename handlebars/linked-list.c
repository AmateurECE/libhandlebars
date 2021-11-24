///////////////////////////////////////////////////////////////////////////////
// NAME:            linked-list.c
//
// AUTHOR:          Ethan D. Twardy <ethan.twardy@gmail.com>
//
// DESCRIPTION:     Implementation of the linked list.
//
// CREATED:         11/22/2021
//
// LAST EDITED:     11/24/2021
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

HbCons* hb_list_push_back(HbList* list, void* user_data)
{
    HbCons* cons = malloc(sizeof(HbCons));
    if (NULL == cons) {
        return NULL;
    }

    cons->next = NULL;
    cons->data = user_data;
    if (NULL == list->head) {
        list->head = cons;
    }

    if (NULL != list->tail) {
        list->tail->next = cons;
    }
    list->tail = cons;

    list->length += 1;
    return cons;
}

void hb_list_free(HbList** list, void (*free_data)(void*)) {
    if (NULL == *list) {
        return;
    }

    HbCons* current = NULL;
    HbCons* next = (*list)->head;
    while (NULL != next) {
        current = next;
        if (NULL != free_data) {
            free_data(current->data);
        }
        next = current->next;
        free(current);
    }

    free(*list);
    *list = NULL;
}

///////////////////////////////////////////////////////////////////////////////
