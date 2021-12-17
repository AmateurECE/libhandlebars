///////////////////////////////////////////////////////////////////////////////
// NAME:            nary-tree.c
//
// AUTHOR:          Ethan D. Twardy <ethan.twardy@gmail.com>
//
// DESCRIPTION:     Implementation of the N-ary tree
//
// CREATED:         12/17/2021
//
// LAST EDITED:     12/17/2021
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
#include <string.h>

#include <handlebars/nary-tree.h>
#include <handlebars/vector.h>

typedef struct HbNaryNode {
    int parent;
    void* user_data;
    void (*free)(void* user_data);
} HbNaryNode;

typedef struct HbNaryTree {
    HbVector* nodes;
} HbNaryTree;

typedef struct HbNaryNodeIterator {
    int index;
    HbNaryTree* tree;
} HbNaryNodeIterator;

typedef void VectorFreeFn(void*);

///////////////////////////////////////////////////////////////////////////////
// Public API
////

HbNaryTree* hb_nary_tree_new() {
    HbNaryTree* tree = malloc(sizeof(HbNaryTree));
    if (NULL == tree) {
        return NULL;
    }

    memset(tree, 0, sizeof(HbNaryTree));
    tree->nodes = hb_vector_init();
    return tree;
}

void hb_nary_tree_free(HbNaryTree** tree) {
    if (NULL == *tree) {
        return;
    }

    hb_vector_free(&(*tree)->nodes, (VectorFreeFn*)hb_nary_node_free);
    free(*tree);
    *tree = NULL;
}

HbNaryNode* hb_nary_tree_get_root(HbNaryTree* tree)
{ return tree->nodes->vector[tree->nodes->length - 1]; }

HbNaryNode* hb_nary_node_new(void* user_data, void(*free)(void* user_data)) {
    HbNaryNode* node = malloc(sizeof(HbNaryNode));
    if (NULL == node) {
        return NULL;
    }

    memset(node, 0, sizeof(HbNaryNode));
    node->user_data = user_data;
    node->free = free;
    return node;
}

int hb_nary_node_append_child(HbNaryTree* tree, HbNaryNode* parent,
    HbNaryNode* child)
{ return 1; }

HbNaryNode* hb_nary_node_get_parent(HbNaryTree* tree, HbNaryNode* node)
{ return tree->nodes->vector[node->parent]; }

void hb_nary_node_free(HbNaryNode* node) {
    if (NULL != node->free) {
        node->free(node->user_data);
    }
    free(node);
}

void hb_nary_node_iterator_init(HbNaryNodeIterator* iter, HbNaryTree* tree) {
    iter->index = 0;
    iter->tree = tree;
}

HbNaryNode* hb_nary_node_iterator_next(HbNaryNodeIterator* iter)
{ return iter->tree->nodes->vector[iter->index++]; }

///////////////////////////////////////////////////////////////////////////////
