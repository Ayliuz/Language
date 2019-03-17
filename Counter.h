// COUNTER.H

#ifndef COUNTER_H_INCLUDED
#define COUNTER_H_INCLUDED

#include <stdio.h>
#include <stdlib.h>
#include <cstdlib>
#include <iostream>
#include <assert.h>
#include <string.h>
#include <math.h>
#include <ctype.h>
#include "ZulanTree.h"


Node* operate_branch_create(unsigned int *out_tree_ver_num, tree_type oper, Node* value1, Node* value2);

tree_type AST_count(Tree* AST);
tree_type leaf_counter(Node* leaf);

Node* AST_diff(char* variable, Tree* AST, Node* start_node);
Node* leaf_diff(int var_index, Node* node, int *out_vertex_num);

Tree* AST_simplify(Tree* AST, Node* start_node);
int leaf_simplify(Node* node, int *out_vertex_num);

int AST_print(const char PRINTNAME[], Tree* AST);
void tex_begin(FILE* write_file);

int leaf_print(FILE* print_file, Tree* AST, Node* node);
int operation_priority(int operation);

#endif // COUNTER_H_INCLUDED
