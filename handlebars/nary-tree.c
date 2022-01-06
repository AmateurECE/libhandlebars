///////////////////////////////////////////////////////////////////////////////
// NAME:            nary-tree.c
//
// AUTHOR:          Ethan D. Twardy <ethan.twardy@gmail.com>
//
// DESCRIPTION:     Implementation of the N-ary tree
//
// CREATED:         12/17/2021
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

#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include <handlebars/nary-tree.h>
#include <handlebars/vector.h>

typedef struct HbsNaryNode {
    HbsNaryNode* parent;
    void* user_data;
    void (*free)(void* user_data);
} HbsNaryNode;

typedef struct HbsNaryTree {
    HbsVector* nodes;
} HbsNaryTree;

///////////////////////////////////////////////////////////////////////////////
// Public API
////

HbsNaryTree* hbs_nary_tree_new() {
    HbsNaryTree* tree = malloc(sizeof(HbsNaryTree));
    if (NULL == tree) {
        return NULL;
    }

    memset(tree, 0, sizeof(HbsNaryTree));
    tree->nodes = hbs_vector_new();
    return tree;
}

void hbs_nary_tree_free(HbsNaryTree* tree) {
    hbs_vector_free(tree->nodes, (VectorFreeDataFn*)hbs_nary_node_free);
    free(tree);
}

HbsNaryNode* hbs_nary_tree_get_root(HbsNaryTree* tree)
{ return tree->nodes->vector[tree->nodes->length - 1]; }

void hbs_nary_tree_set_root(HbsNaryTree* tree, HbsNaryNode* node) {
    if (0 == tree->nodes->length) {
        hbs_vector_push_back(tree->nodes, node);
    } else {
        tree->nodes->vector[tree->nodes->length - 1] = node;
    }
}

int hbs_nary_tree_append_child_to_node(HbsNaryTree* tree, HbsNaryNode* parent,
    HbsNaryNode* child)
{
    size_t index = 0;
    for (index = tree->nodes->length - 1; index != SIZE_MAX; --index) {
        HbsNaryNode* node = tree->nodes->vector[index];
        if (node->parent == parent) {
            break;
        }
    }

    hbs_vector_insert(tree->nodes, index + 1, child);
    child->parent = parent;
    return 0;
}

HbsNaryNode* hbs_nary_node_new(void* user_data, void(*free)(void* user_data)) {
    HbsNaryNode* node = malloc(sizeof(HbsNaryNode));
    if (NULL == node) {
        return NULL;
    }

    memset(node, 0, sizeof(HbsNaryNode));
    node->user_data = user_data;
    node->free = free;
    return node;
}

HbsNaryNode* hbs_nary_node_get_parent(HbsNaryNode* node)
{ return node->parent; }

void* hbs_nary_node_get_data(HbsNaryNode* node)
{ return node->user_data; }

void hbs_nary_node_free(HbsNaryNode* node) {
    if (NULL != node->free) {
        node->free(node->user_data);
    }
    free(node);
}

void hbs_nary_tree_iter_init(HbsNaryTreeIter* iter, HbsNaryTree* tree) {
    iter->index = 0;
    iter->tree = tree;
}

HbsNaryNode* hbs_nary_tree_iter_next(HbsNaryTreeIter* iter) {
    if (iter->index >= iter->tree->nodes->length) {
        return NULL;
    }

    return iter->tree->nodes->vector[iter->index++];
}

///////////////////////////////////////////////////////////////////////////////
