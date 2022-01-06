///////////////////////////////////////////////////////////////////////////////
// NAME:            test-parser.c
//
// AUTHOR:          Ethan D. Twardy <ethan.twardy@gmail.com>
//
// DESCRIPTION:     Tests for the parser.
//
// CREATED:         01/03/2022
//
// LAST EDITED:     01/06/2022
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

static HbsInputContext* input_context;
static HbsScanner* scanner;
static HbsParser* parser;
static HbsNaryTree* tree;

TEST_GROUP(HbsParser);

TEST_SETUP(HbsParser) {
    tree = NULL;
}

TEST_TEAR_DOWN(HbsParser) {
    if (NULL != parser) {
        hbs_parser_free(parser);
    }

    if (NULL != scanner) {
        hbs_scanner_free(scanner);
    }

    if (NULL != input_context) {
        hbs_input_context_free(input_context);
    }

    if (NULL != tree) {
        hbs_nary_tree_free(tree);
    }
}

static void parser_verification_setup(const char* string) {
    input_context = hbs_input_context_from_string(string);
    scanner = hbs_scanner_new(input_context);
    parser = hbs_parser_new(scanner);
}

static void parser_check_text_component(HbsNaryTreeIter* iterator,
    const char* string)
{
    HbsNaryNode* element = hbs_nary_tree_iter_next(iterator);
    TEST_ASSERT_NOT_NULL(element);
    HbsComponent* component = (HbsComponent*)hbs_nary_node_get_data(element);
    TEST_ASSERT_NOT_NULL(component);
    TEST_ASSERT_EQUAL_INT(HBS_COMPONENT_TEXT, component->type);
    TEST_ASSERT_NOT_NULL(component->text);
    TEST_ASSERT_EQUAL_STRING(string, component->text->string);
}

static void parser_check_expression_component(HbsNaryTreeIter* iterator,
    const size_t length, const char* argv[length])
{
    HbsNaryNode* element = hbs_nary_tree_iter_next(iterator);
    TEST_ASSERT_NOT_NULL(element);
    HbsComponent* component = (HbsComponent*)hbs_nary_node_get_data(element);
    TEST_ASSERT_NOT_NULL(component);
    TEST_ASSERT_EQUAL_INT(HBS_COMPONENT_EXPRESSION, component->type);
    TEST_ASSERT_NOT_NULL(component->argv);
    TEST_ASSERT_EQUAL_INT(length, component->argv->length);
    for (size_t i = 0; i < length; ++i) {
        HbsString* argument = (HbsString*)component->argv->vector[i];
        TEST_ASSERT_EQUAL_STRING(argv[i], argument->string);
    }
}

static void parser_check_root(HbsNaryTreeIter* iterator) {
    HbsNaryNode* element = hbs_nary_tree_iter_next(iterator);
    HbsNaryNode* root = hbs_nary_tree_get_root(tree);
    TEST_ASSERT_EQUAL(element, root);
}

static const char* TEXT_TEST = "The quick brown fox";
TEST(HbsParser, Text) {
    parser_verification_setup(TEXT_TEST);
    TEST_ASSERT_EQUAL_INT(0, hbs_parser_parse(parser, &tree));
    TEST_ASSERT_NOT_NULL(tree);

    HbsNaryTreeIter iterator;
    hbs_nary_tree_iter_init(&iterator, tree);
    parser_check_text_component(&iterator, TEXT_TEST);
    parser_check_root(&iterator);
}

static const char* HANDLEBARS_TEST = "{{test}}";
TEST(HbsParser, Handlebars) {
    parser_verification_setup(HANDLEBARS_TEST);
    TEST_ASSERT_EQUAL_INT(0, hbs_parser_parse(parser, &tree));
    TEST_ASSERT_NOT_NULL(tree);

    HbsNaryTreeIter iterator;
    hbs_nary_tree_iter_init(&iterator, tree);
    static const char* argv[] = {"test"};
    parser_check_expression_component(&iterator,
        sizeof(argv) / sizeof(argv[0]), argv);
    parser_check_root(&iterator);
}

static const char* COMBINATION_TEST = "The {{quick}} brown fox";
TEST(HbsParser, Combination) {
    parser_verification_setup(COMBINATION_TEST);
    TEST_ASSERT_EQUAL_INT(0, hbs_parser_parse(parser, &tree));
    TEST_ASSERT_NOT_NULL(tree);

    HbsNaryTreeIter iterator;
    hbs_nary_tree_iter_init(&iterator, tree);
    parser_check_text_component(&iterator, "The ");
    static const char* argv[] = {"quick"};
    parser_check_expression_component(&iterator,
        sizeof(argv) / sizeof(argv[0]), argv);
    parser_check_text_component(&iterator, " brown fox");
    parser_check_root(&iterator);
}

static const char* MULTIPLE_TEST = "{{test1 test2}}";
TEST(HbsParser, Multiple) {
    parser_verification_setup(MULTIPLE_TEST);
    TEST_ASSERT_EQUAL_INT(0, hbs_parser_parse(parser, &tree));
    TEST_ASSERT_NOT_NULL(tree);

    HbsNaryTreeIter iterator;
    hbs_nary_tree_iter_init(&iterator, tree);
    static const char* argv[] = {"test1", "test2"};
    parser_check_expression_component(&iterator,
        sizeof(argv) / sizeof(argv[0]), argv);
    parser_check_root(&iterator);
}

static const char* UNCLOSED_EXPRESSION_ERROR_TEST = "{{test";
TEST(HbsParser, UnclosedExpressionError) {
    parser_verification_setup(UNCLOSED_EXPRESSION_ERROR_TEST);
    TEST_ASSERT_EQUAL_INT(1, hbs_parser_parse(parser, &tree));
    TEST_ASSERT_NULL(tree);
}

static const char* MISMATCHED_HANDLEBARS_ERROR_TEST = "}}";
TEST(HbsParser, MismatchedHandlebarsError) {
    parser_verification_setup(MISMATCHED_HANDLEBARS_ERROR_TEST);
    TEST_ASSERT_EQUAL_INT(1, hbs_parser_parse(parser, &tree));
    TEST_ASSERT_NULL(tree);
}

static const char* NESTED_EXPRESSION_ERROR_TEST = "{{test {{";
TEST(HbsParser, NestedExpressionError) {
    parser_verification_setup(NESTED_EXPRESSION_ERROR_TEST);
    TEST_ASSERT_EQUAL_INT(1, hbs_parser_parse(parser, &tree));
    TEST_ASSERT_NULL(tree);
}

static const char* EMPTY_EXPRESSION_ERROR_TEST = "{{}}";
TEST(HbsParser, EmptyExpressionError) {
    parser_verification_setup(EMPTY_EXPRESSION_ERROR_TEST);
    TEST_ASSERT_EQUAL_INT(1, hbs_parser_parse(parser, &tree));
    TEST_ASSERT_NULL(tree);
}

TEST_GROUP_RUNNER(HbsParser) {
    RUN_TEST_CASE(HbsParser, Text);
    RUN_TEST_CASE(HbsParser, Handlebars);
    RUN_TEST_CASE(HbsParser, Combination);
    RUN_TEST_CASE(HbsParser, Multiple);
    RUN_TEST_CASE(HbsParser, UnclosedExpressionError);
    RUN_TEST_CASE(HbsParser, MismatchedHandlebarsError);
    RUN_TEST_CASE(HbsParser, NestedExpressionError);
    RUN_TEST_CASE(HbsParser, EmptyExpressionError);
}

///////////////////////////////////////////////////////////////////////////////
