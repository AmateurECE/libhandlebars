///////////////////////////////////////////////////////////////////////////////
// NAME:            nary-tree.h
//
// AUTHOR:          Ethan D. Twardy <ethan.twardy@gmail.com>
//
// DESCRIPTION:     Interface for the Handlebars N-ary tree implementation
//                  (used for abstract syntax tree generation)
//
// CREATED:         12/17/2021
//
// LAST EDITED:     01/04/2022
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

#ifndef HANDLEBARS_NARY_TREE_H
#define HANDLEBARS_NARY_TREE_H

typedef struct HbNaryNode HbNaryNode;
typedef struct HbNaryTree HbNaryTree;
typedef struct HbNaryNodeIter {
    size_t index;
    HbNaryTree* tree;
} HbNaryNodeIter;


HbNaryTree* hb_nary_tree_new();
void hb_nary_tree_free(HbNaryTree** tree);
HbNaryNode* hb_nary_tree_get_root(HbNaryTree* tree);
void hb_nary_tree_set_root(HbNaryTree* tree, HbNaryNode* node);
int hb_nary_tree_append_child_to_node(HbNaryTree* tree, HbNaryNode* parent,
    HbNaryNode* child);

HbNaryNode* hb_nary_node_new(void* user_data, void(*free)(void* user_data));
HbNaryNode* hb_nary_node_get_parent(HbNaryNode* node);
void hb_nary_node_free(HbNaryNode* node);
void* hb_nary_node_get_data(HbNaryNode* node);

void hb_nary_node_iter_init(HbNaryNodeIter* iter, HbNaryTree* tree);
HbNaryNode* hb_nary_node_iter_next(HbNaryNodeIter* iter);

#endif // HANDLEBARS_NARY_TREE_H

///////////////////////////////////////////////////////////////////////////////
