// TREE.H

#ifndef TREE_H_INCLUDED
#define TREE_H_INCLUDED

#include <stdio.h>
#include <stdlib.h>
#include <cstdlib>
#include <iostream>
#include <assert.h>
#include <string.h>
#include <math.h>
#include <ctype.h>
#include <direct.h>
#include "stack.h"

#define L 1
#define R 0
#define NOPARENT -1
#define GUARD 7933
#define GUARDPOIS -1
#define HASHPOIS -1
#define HASHDEFAULT 0
#define TRERROK 0
#define TRERREMOVE 1
#define TRERDATA 2
#define TRERPOS 4
#define TRERSTRUCT 5
#define TRERLEN 6
#define TRERVARLEN 7

#ifndef $
#define $(x)            std::cout<<"~"<<#x " = "<<x<<"\n";
#endif

#define PARENT(node) (node)->parent
#define LEFT(node) (node)->left
#define RIGHT(node) (node)->right
#define ROOT(tree)  (tree)->root

#define SET_PARENT(child, parent)       if (child)\
                                        {\
                                            PARENT(child) = parent;\
                                        }

#define SET_CHILDREN(node, left, right)         LEFT(node) = left;\
                                                RIGHT(node) = right;

typedef double tree_type;
#define POISON NAN

typedef unsigned int out_ptr;

const size_t STRLEN = 100;

const size_t START_VAR_NUM = 1000;
const size_t MAX_VAR_LEN = 100;

enum Types
{
    NO_TYPE = 0,
    CONST_T = 1,
    VAR = 2,
    OP = 3
};

enum Operators
{
    SUM = 1,        // '+'
    SUB,            // '-'
    MUL,            // '*'
    DIV,            // '/'
    SIN,
    COS,
    TAN,
    LN,
    EXP,
    POW
};

struct Node
{
    int node_guard_begin = GUARDPOIS;

    Node* parent = NULL;

    tree_type info = NAN;
    int type = NO_TYPE;

    Node* left = NULL;
    Node* right = NULL;

    int node_guard_end = GUARDPOIS;
};

struct Tree
{
    int tree_guard_begin = GUARDPOIS;

    unsigned int ver_num = 0;

    Node* root = NULL;

    char** variables = NULL;
    unsigned int var_count = 0;

    int TreeHash_struct = HASHDEFAULT;

    int tree_guard_end = GUARDPOIS;
};

void tree_test(const char dot[]);

void tree_Ctor(Tree* tree);
void tree_Dtor(Tree* tree);
void free_table (char** table, int n_table);

void node_Ctor(Node* node);
void node_Dtor(Node* node);

void tree_make_hash(Tree* tree);

Node* tree_set_root(Tree* tree, Node* root);
Node* node_create(int type, tree_type val);
Node* branch_copy(Node* start_node, unsigned int* out_vertex_num);
Node* tree_node_replace(Tree* tree, Node* old_node, Node* new_node);
int node_swap(Node* node1, Node* node2);

Node* tree_insert_left(Tree* tree, Node* parent_node, int type, tree_type new_val);
Node* tree_insert_right(Tree* tree, Node* parent_node, int type, tree_type new_val);

Node* node_find(Node* start_node, tree_type val, int (*compare)(tree_type value1, tree_type value2));
int node_define(Node* node);                                            // get side of the node related to the parent
int branch_compare(Node* node1, Node* node2);


Node* tree_find_common(Node* node1, Node* node2, int* out_node1_side);
size_t node_get_height(Node* node);

Tree* tree_clear(Tree* tree);
int node_cut(Node* node, int* out_vertex_num);
int tree_erase(Tree* tree, Node* node);

int tree_is_OK(Tree* tree);
int node_is_OK(Node* node, unsigned int* counter);

int tree_dump(const char dot[], const char DUMPNAME[], Tree* tree);
int node_dump(FILE* dump_file, Tree* AST,  Node* start_node);
int tree_draw(const char dot[], const char DUMPNAME[], Tree* tree);

void tree_save(Tree* tree, const char PRINTNAME[]);
void node_save(Tree* AST, Node* start_node, FILE* print_file);

Tree* tree_read(const char READNAME[]);
Node* node_read(FILE* read_file, Tree* tree, int* error);
Node* get_node(FILE* read_file, Tree* tree);
int find_elem(char** arr, int N_arr, const char* str);

#endif // TREE_H_INCLUDED
