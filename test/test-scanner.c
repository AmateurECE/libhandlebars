///////////////////////////////////////////////////////////////////////////////
// NAME:            test-scanner.c
//
// AUTHOR:          Ethan D. Twardy <ethan.twardy@gmail.com>
//
// DESCRIPTION:     Unit tests for the HbsScanner.
//
// CREATED:         12/29/2021
//
// LAST EDITED:     01/06/2022
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

#include <unity_fixture.h>

#include <handlebars/handlebars.h>
#include <handlebars/scanner.h>

static HbsInputContext* input_context;
static HbsScanner* scanner;
static HbsParseToken token;

TEST_GROUP(HbsScanner);
TEST_SETUP(HbsScanner) {
    input_context = NULL;
    scanner = NULL;
    memset(&token, 0, sizeof(HbsParseToken));
}

TEST_TEAR_DOWN(HbsScanner) {
    if (NULL != scanner) {
        hbs_scanner_free(scanner);
    }

    if (NULL != input_context) {
        hbs_input_context_free(input_context);
    }
}

void scanner_token_verification_setup(const char* test_string) {
    input_context = hbs_input_context_from_string(test_string);
    scanner = hbs_scanner_new(input_context);
}

void scanner_token_compare(HbsParseTokenType type, const char* string, int line,
    int column)
{
    TEST_ASSERT_EQUAL_INT(1, hbs_scanner_next_symbol(scanner, &token));
    TEST_ASSERT_EQUAL_INT(type, token.type);

    if (NULL != string) {
        TEST_ASSERT_EQUAL_STRING(string, token.string->string);
    }

    if (-1 != line) {
        TEST_ASSERT_EQUAL_INT(line, token.line);
    }

    if (-1 != column) {
        TEST_ASSERT_EQUAL_INT(column, token.column);
    }

    hbs_token_release(&token);
}

static const char* BASIC_TEST = "The quick brown fox jumped over the lazy dog";
TEST(HbsScanner, Basic) {
    scanner_token_verification_setup(BASIC_TEST);
    scanner_token_compare(HBS_TOKEN_TEXT, BASIC_TEST, 1, 0);
    TEST_ASSERT_EQUAL_INT(1, hbs_scanner_next_symbol(scanner, &token));
    TEST_ASSERT_EQUAL_INT(HBS_TOKEN_EOF, token.type);
}

static const char* TOKEN_TEST = "Text {{ whitespace }}.";
TEST(HbsScanner, Token) {
    scanner_token_verification_setup(TOKEN_TEST);
    scanner_token_compare(HBS_TOKEN_TEXT, "Text ", 1, 0);
    scanner_token_compare(HBS_TOKEN_OPEN_BARS, NULL, 1, 5);
    scanner_token_compare(HBS_TOKEN_TEXT, " whitespace ", 1, 7);
    scanner_token_compare(HBS_TOKEN_CLOSE_BARS, NULL, 1, 19);
    scanner_token_compare(HBS_TOKEN_TEXT, ".", 1, 21);
    scanner_token_compare(HBS_TOKEN_EOF, NULL, -1, -1);
}

static const char* WHITESPACE_TEST = "Text {{ whitespace }}.";
TEST(HbsScanner, Whitespace) {
    scanner_token_verification_setup(WHITESPACE_TEST);
    scanner_token_compare(HBS_TOKEN_TEXT, "Text ", 1, 0);
    scanner_token_compare(HBS_TOKEN_OPEN_BARS, NULL, 1, 5);
    hbs_scanner_enable_ws_token(scanner);
    scanner_token_compare(HBS_TOKEN_WS, " ", 1, 7);
    scanner_token_compare(HBS_TOKEN_TEXT, "whitespace", 1, 8);
    scanner_token_compare(HBS_TOKEN_WS, " ", 1, 18);
    hbs_scanner_disable_ws_token(scanner);
    scanner_token_compare(HBS_TOKEN_CLOSE_BARS, NULL, 1, 19);
    scanner_token_compare(HBS_TOKEN_TEXT, ".", 1, 21);
    scanner_token_compare(HBS_TOKEN_EOF, NULL, -1, -1);
}

static const char* EOF_TEST = "";
TEST(HbsScanner, Eof) {
    scanner_token_verification_setup(EOF_TEST);
    scanner_token_compare(HBS_TOKEN_EOF, NULL, 1, 0);
}

static const char* DOUBLE_WHITESPACE = "Text  text";
TEST(HbsScanner, DoubleWhitespace) {
    scanner_token_verification_setup(DOUBLE_WHITESPACE);
    hbs_scanner_enable_ws_token(scanner);
    scanner_token_compare(HBS_TOKEN_TEXT, "Text", 1, 0);
    scanner_token_compare(HBS_TOKEN_WS, "  ", 1, 4);
    scanner_token_compare(HBS_TOKEN_TEXT, "text", 1, 6);
    scanner_token_compare(HBS_TOKEN_EOF, NULL, -1, -1);
}

TEST_GROUP_RUNNER(HbsScanner) {
    RUN_TEST_CASE(HbsScanner, Basic);
    RUN_TEST_CASE(HbsScanner, Token);
    RUN_TEST_CASE(HbsScanner, Whitespace);
    RUN_TEST_CASE(HbsScanner, Eof);
    RUN_TEST_CASE(HbsScanner, DoubleWhitespace);
}

///////////////////////////////////////////////////////////////////////////////
