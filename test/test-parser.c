///////////////////////////////////////////////////////////////////////////////
// NAME:            test-parser.c
//
// AUTHOR:          Ethan D. Twardy <ethan.twardy@gmail.com>
//
// DESCRIPTION:     Tests for the parser.
//
// CREATED:         01/03/2022
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
#include <handlebars/nary-tree.h>
#include <handlebars/parser.h>
#include <handlebars/scanner.h>

static HbInputContext* input_context;
static HbScanner* scanner;
static HbParser* parser;
static HbNaryTree* tree;

TEST_GROUP(HbParser);

TEST_SETUP(HbParser) {
    tree = NULL;
}
TEST_TEAR_DOWN(HbParser) {}

static void parser_verification_setup(const char* string) {
    input_context = handlebars_input_context_from_string(string);
    scanner = hb_scanner_new(input_context);
    parser = hb_parser_new(scanner);
}

static void parser_check_text_component(HbNaryTreeIter* iterator,
    const char* string)
{
    HbNaryNode* element = hb_nary_tree_iter_next(iterator);
    TEST_ASSERT_NOT_NULL(element);
    HbComponent* component = (HbComponent*)hb_nary_node_get_data(element);
    TEST_ASSERT_NOT_NULL(component);
    TEST_ASSERT_EQUAL_INT(HB_COMPONENT_TEXT, component->type);
    TEST_ASSERT_EQUAL_STRING(string, component->text->string);
}

static void parser_check_root(HbNaryTreeIter* iterator) {
    HbNaryNode* element = hb_nary_tree_iter_next(iterator);
    HbNaryNode* root = hb_nary_tree_get_root(tree);
    TEST_ASSERT_EQUAL(element, root);
}

static const char* TEXT_TEST = "The quick brown fox";
TEST(HbParser, Text) {
    parser_verification_setup(TEXT_TEST);
    TEST_ASSERT_EQUAL_INT(0, hb_parser_parse(parser, &tree));
    TEST_ASSERT_NOT_NULL(tree);

    HbNaryTreeIter iterator;
    hb_nary_tree_iter_init(&iterator, tree);
    parser_check_text_component(&iterator, TEXT_TEST);
    parser_check_root(&iterator);
}

TEST_GROUP_RUNNER(HbParser) {
    RUN_TEST_CASE(HbParser, Text);
}

///////////////////////////////////////////////////////////////////////////////
