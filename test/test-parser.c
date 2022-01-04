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
#include <handlebars/vector.h>

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
    TEST_ASSERT_NOT_NULL(component->text);
    TEST_ASSERT_EQUAL_STRING(string, component->text->string);
}

static void parser_check_expression_component(HbNaryTreeIter* iterator,
    const size_t length, const char* argv[length])
{
    HbNaryNode* element = hb_nary_tree_iter_next(iterator);
    TEST_ASSERT_NOT_NULL(element);
    HbComponent* component = (HbComponent*)hb_nary_node_get_data(element);
    TEST_ASSERT_NOT_NULL(component);
    TEST_ASSERT_EQUAL_INT(HB_COMPONENT_EXPRESSION, component->type);
    TEST_ASSERT_NOT_NULL(component->argv);
    TEST_ASSERT_EQUAL_INT(length, component->argv->length);
    for (size_t i = 0; i < length; ++i) {
        HbString* argument = (HbString*)component->argv->vector[i];
        TEST_ASSERT_EQUAL_STRING(argv[i], argument->string);
    }
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

static const char* HANDLEBARS_TEST = "{{test}}";
TEST(HbParser, Handlebars) {
    parser_verification_setup(HANDLEBARS_TEST);
    TEST_ASSERT_EQUAL_INT(0, hb_parser_parse(parser, &tree));
    TEST_ASSERT_NOT_NULL(tree);

    HbNaryTreeIter iterator;
    hb_nary_tree_iter_init(&iterator, tree);
    static const char* argv[] = {"test"};
    parser_check_expression_component(&iterator,
        sizeof(argv) / sizeof(argv[0]), argv);
    parser_check_root(&iterator);
}

static const char* COMBINATION_TEST = "The {{quick}} brown fox";
TEST(HbParser, Combination) {
    parser_verification_setup(COMBINATION_TEST);
    TEST_ASSERT_EQUAL_INT(0, hb_parser_parse(parser, &tree));
    TEST_ASSERT_NOT_NULL(tree);

    HbNaryTreeIter iterator;
    hb_nary_tree_iter_init(&iterator, tree);
    parser_check_text_component(&iterator, "The ");
    static const char* argv[] = {"quick"};
    parser_check_expression_component(&iterator,
        sizeof(argv) / sizeof(argv[0]), argv);
    parser_check_text_component(&iterator, " brown fox");
    parser_check_root(&iterator);
}

static const char* MULTIPLE_TEST = "{{test1 test2}}";
TEST(HbParser, Multiple) {
    parser_verification_setup(MULTIPLE_TEST);
    TEST_ASSERT_EQUAL_INT(0, hb_parser_parse(parser, &tree));
    TEST_ASSERT_NOT_NULL(tree);

    HbNaryTreeIter iterator;
    hb_nary_tree_iter_init(&iterator, tree);
    static const char* argv[] = {"test1", "test2"};
    parser_check_expression_component(&iterator,
        sizeof(argv) / sizeof(argv[0]), argv);
    parser_check_root(&iterator);
}

static const char* UNCLOSED_EXPRESSION_ERROR_TEST = "{{test";
TEST(HbParser, UnclosedExpressionError) {
    parser_verification_setup(UNCLOSED_EXPRESSION_ERROR_TEST);
    TEST_ASSERT_EQUAL_INT(1, hb_parser_parse(parser, &tree));
    TEST_ASSERT_NULL(tree);
}

static const char* MISMATCHED_HANDLEBARS_ERROR_TEST = "}}";
TEST(HbParser, MismatchedHandlebarsError) {
    parser_verification_setup(MISMATCHED_HANDLEBARS_ERROR_TEST);
    TEST_ASSERT_EQUAL_INT(1, hb_parser_parse(parser, &tree));
    TEST_ASSERT_NULL(tree);
}

static const char* NESTED_EXPRESSION_ERROR_TEST = "{{test {{";
TEST(HbParser, NestedExpressionError) {
    parser_verification_setup(NESTED_EXPRESSION_ERROR_TEST);
    TEST_ASSERT_EQUAL_INT(1, hb_parser_parse(parser, &tree));
    TEST_ASSERT_NULL(tree);
}

TEST_GROUP_RUNNER(HbParser) {
    RUN_TEST_CASE(HbParser, Text);
    RUN_TEST_CASE(HbParser, Handlebars);
    RUN_TEST_CASE(HbParser, Combination);
    RUN_TEST_CASE(HbParser, Multiple);
    RUN_TEST_CASE(HbParser, UnclosedExpressionError);
    RUN_TEST_CASE(HbParser, MismatchedHandlebarsError);
    RUN_TEST_CASE(HbParser, NestedExpressionError);
}

///////////////////////////////////////////////////////////////////////////////
