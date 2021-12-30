///////////////////////////////////////////////////////////////////////////////
// NAME:            test-scanner.c
//
// AUTHOR:          Ethan D. Twardy <ethan.twardy@gmail.com>
//
// DESCRIPTION:     Unit tests for the HbScanner.
//
// CREATED:         12/29/2021
//
// LAST EDITED:     12/29/2021
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

#include <string.h>

#include <unity.h>

#include <handlebars/handlebars.h>
#include <handlebars/scanner.h>
#include "test-scanner.h"

static HbInputContext* input_context;
static HbScanner* scanner;
static HbParseToken token;

void setUp() {
    input_context = NULL;
    scanner = NULL;
    memset(&token, 0, sizeof(HbParseToken));
}

void tearDown() {
    hb_scanner_free(scanner);
}

void scanner_token_verification_setup(const char* test_string) {
    input_context = handlebars_input_context_from_string(test_string);
    scanner = hb_scanner_new(input_context);
}

static const char* BASIC_TEST = "The quick brown fox jumped over the lazy dog";
void test_HbScanner_Basic() {
    scanner_token_verification_setup(BASIC_TEST);
    TEST_ASSERT_EQUAL_INT(1, hb_scanner_next_symbol(scanner, &token));
    TEST_ASSERT_EQUAL_INT(HB_TOKEN_TEXT, token.type);
    TEST_ASSERT_EQUAL_STRING(BASIC_TEST, token.string->string);
    TEST_ASSERT_EQUAL_INT(1, token.line);
    TEST_ASSERT_EQUAL_INT(0, token.column);

    hb_token_release(&token);
    TEST_ASSERT_EQUAL_INT(1, hb_scanner_next_symbol(scanner, &token));
    TEST_ASSERT_EQUAL_INT(HB_TOKEN_EOF, token.type);
}

///////////////////////////////////////////////////////////////////////////////
