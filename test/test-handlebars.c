///////////////////////////////////////////////////////////////////////////////
// NAME:            test-handlebars.c
//
// AUTHOR:          Ethan D. Twardy <ethan.twardy@gmail.com>
//
// DESCRIPTION:     Tests for the high-level Handlebars rendering API.
//
// CREATED:         01/04/2022
//
// LAST EDITED:     01/04/2022
//
// Copyright 2022, Ethan D. Twardy
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

#include <unity_fixture.h>

#include <handlebars/handlebars.h>

TEST_GROUP(Handlebars);
TEST_SETUP(Handlebars) {}
TEST_TEAR_DOWN(Handlebars) {}

static const char* BASIC_TEST = "The {{quick}} brown fox";
TEST(Handlebars, Basic) {
    HbInputContext* input = handlebars_input_context_from_string(BASIC_TEST);
    Handlebars* template = handlebars_template_load(input);
    TEST_ASSERT_NOT_NULL(template);
    HbTemplateContext* context = handlebars_template_context_init();
    handlebars_template_context_set_string(context, "quick", "sneaky");
    HbString* output = handlebars_template_render(template, context);
    TEST_ASSERT_EQUAL_STRING("The sneaky brown fox", output->string);
    hb_string_free(&output);
    handlebars_template_context_free(&context);
    handlebars_template_free(&template);
}

TEST_GROUP_RUNNER(Handlebars) {
    RUN_TEST_CASE(Handlebars, Basic);
}

///////////////////////////////////////////////////////////////////////////////
