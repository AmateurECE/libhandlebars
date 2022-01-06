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

#ifndef HANDLEBARS_NARY_TREE_H
#define HANDLEBARS_NARY_TREE_H

typedef struct HbsNaryNode HbsNaryNode;
typedef struct HbsNaryTree HbsNaryTree;
typedef struct HbsNaryTreeIter {
    size_t index;
    HbsNaryTree* tree;
} HbsNaryTreeIter;


HbsNaryTree* hbs_nary_tree_new();
void hbs_nary_tree_free(HbsNaryTree** tree);
HbsNaryNode* hbs_nary_tree_get_root(HbsNaryTree* tree);
void hbs_nary_tree_set_root(HbsNaryTree* tree, HbsNaryNode* node);
int hbs_nary_tree_append_child_to_node(HbsNaryTree* tree, HbsNaryNode* parent,
    HbsNaryNode* child);

HbsNaryNode* hbs_nary_node_new(void* user_data, void(*free)(void* user_data));
HbsNaryNode* hbs_nary_node_get_parent(HbsNaryNode* node);
void hbs_nary_node_free(HbsNaryNode* node);
void* hbs_nary_node_get_data(HbsNaryNode* node);

void hbs_nary_tree_iter_init(HbsNaryTreeIter* iter, HbsNaryTree* tree);
HbsNaryNode* hbs_nary_tree_iter_next(HbsNaryTreeIter* iter);

#endif // HANDLEBARS_NARY_TREE_H

///////////////////////////////////////////////////////////////////////////////
