///////////////////////////////////////////////////////////////////////////////
// NAME:            test-handlebars.c
//
// AUTHOR:          Ethan D. Twardy <ethan.twardy@gmail.com>
//
// DESCRIPTION:     Tests for the high-level Handlebars rendering API.
//
// CREATED:         01/04/2022
//
// LAST EDITED:     01/05/2022
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

#include <string.h>

#include <unity_fixture.h>

#include <handlebars/handlebars.h>

TEST_GROUP(HbTemplate);
TEST_SETUP(HbTemplate) {}
TEST_TEAR_DOWN(HbTemplate) {}

static HbResult basic_key_handler(void* user_data __attribute__((unused)),
    const char* key, const char** value)
{
    TEST_ASSERT_EQUAL_STRING("quick", key);
    *value = "sneaky";
    return HB_OK;
}

static const char* BASIC_TEST = "The {{quick}} brown fox";
TEST(HbTemplate, Basic) {
    HbInputContext* input = hb_input_context_from_string(BASIC_TEST);
    HbTemplate* template = hb_template_load(input);
    TEST_ASSERT_NOT_NULL(template);

    HbHandlers handlers = {
        .key_handler = basic_key_handler,
        .key_handler_data = NULL,
    };
    HbString* result = hb_template_render(template, &handlers);
    TEST_ASSERT_NOT_NULL(result);
    static const char* rendered_result = "The sneaky brown fox";
    TEST_ASSERT_EQUAL_STRING(rendered_result, result->string);
    TEST_ASSERT_EQUAL_INT(strlen(rendered_result), result->length);

    hb_string_free(&result);
    hb_template_free(&template);
    hb_input_context_free(&input);
}

TEST_GROUP_RUNNER(HbTemplate) {
    RUN_TEST_CASE(HbTemplate, Basic);
}

///////////////////////////////////////////////////////////////////////////////
