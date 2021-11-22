///////////////////////////////////////////////////////////////////////////////
// NAME:            state-machine.c
//
// AUTHOR:          Ethan D. Twardy <ethan.twardy@gmail.com>
//
// DESCRIPTION:     Implementation of the state machine
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

#include <assert.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <fsautomata/mealy.h>

#include <handlebars/handlebars.h>
#include <handlebars/linked-list.h>
#include <handlebars/state-machine.h>
#include <handlebars/string.h>

static const StateTransition text_tt[] = {
    { SIGNAL_DOCUMENT_START, 0, STATE_TEXT },
    { SIGNAL_DOCUMENT_END, 0, STATE_FINAL },
    { SIGNAL_START_TOKEN, 0, STATE_TEMPLATE },
    { /* sentinel */ },
};

static const StateTransition final_tt[] = { { /* sentinel */ } };
static const int final_states[] = { STATE_FINAL, 0, };
static const MealyState state_table[] = {
    [STATE_TEXT]={ STATE_TEXT, NULL, NULL, hb_read_event, text_tt },
    [STATE_FINAL]={ STATE_FINAL, NULL, NULL, hb_read_event, final_tt },
    { /* sentinel */ },
};

static void print_error(enum FsmFault fault) {
    // TODO: Put more effort into this, since this occurs when a parse error
    // has happened.
    fprintf(stderr, "FSM Fault: %s\n", fsm_strerror(fault));
}

void hb_event(Handlebars* handlebars, enum Event event)
{
    HbParserEvent* parser_event = malloc(sizeof(HbParserEvent));
    assert(NULL != parser_event);
    parser_event->event = event;
    parser_event->string = hb_string_init();

    HbCons* element = hb_list_append(handlebars->parser_events, parser_event);
    // Would normally try to report the error here, but this function is called
    // by the PEG parser.
    assert(NULL != element);
    printf("%d\n", event);
}

void hb_string(Handlebars* handlebars, const char* string)
{
    HbCons* tail = handlebars->parser_events->tail;
    // Would normally try to report the error here, but this function is called
    // by the PEG parser.
    assert(NULL != tail);

    HbParserEvent* event = (HbParserEvent*)tail->data;
    assert(0 == hb_string_append_str(event->string, string));
    printf("'%s'\n", string);
}

int hb_read_event(int* state __attribute__((unused)), void* user_data)
{
    Handlebars* handlebars = (Handlebars*)user_data;
    if (handlebars->parser_events->length < 1) {
        return 0;
    }

    HbCons* element = hb_list_pop_front(handlebars->parser_events);
    assert(NULL != element);
    HbParserEvent* event = element->data;
    assert(NULL != event);
    // TODO: Handle event
    free(event);
    return event->event;
}

MealyFsm* hb_parser_machine_init(Handlebars* handlebars) {
    MealyFsm* machine = malloc(sizeof(MealyFsm));
    if (NULL == machine) {
        return NULL;
    }

    MealyFsm copy_machine = {
        .states = state_table,
        .initial_state = STATE_TEXT,
        .final_states = final_states,
        .user_data = handlebars,
    };
    memcpy(machine, &copy_machine, sizeof(MealyFsm));

    mealy_fsm_initialize(machine);
    return machine;
}

int hb_parser_machine_iterate(MealyFsm* machine) {
    enum FsmEvent fsm_event = mealy_fsm_poll(machine);
    switch (fsm_event) {
    case FSM_EVENT_NONE:
    case FSM_EVENT_STATE_CHANGE:
        return 1;
    case FSM_EVENT_ACCEPTED:
        return 0;
    case FSM_EVENT_FAULT:
        print_error(machine->fault);
        return machine->fault;
    default:
        assert(false);
    }
}

void hb_parser_machine_free(MealyFsm** machine) {
    if (NULL != *machine) {
        free(*machine);
        *machine = NULL;
    }
}

///////////////////////////////////////////////////////////////////////////////
