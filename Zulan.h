// ZULAN . H

#ifndef ZULAN_H_INCLUDED
#define ZULAN_H_INCLUDED

#ifndef $
#define $(x) std::cout<<"~"<<#x " = "<<x<<"\n";
#endif

#include <stdio.h>
#include <stdlib.h>
#include <cstdlib>
#include <iostream>
#include <assert.h>
#include <string.h>
#include <math.h>
#include <ctype.h>
#include "ZulanTree.h"
#include "CPU.h"
#include "Counter.h"

#define SynAssert(function,expr,extrabody)    if(!(expr))\
                                    {\
                                        printf("Syntax error: "#function", \"%c\", str %i, char %i\n", *S, Str_Num, S - BEGIN + 1);\
                                        CountError = 1;\
                                        extrabody;\
                                        return NULL;\
                                    }
#define DEBUG(str)                  if (DEBUG)\
                                    {\
                                        char func[] = #str;\
                                        if(func[0] == '\\')\
                                            DEB_TAB--;\
                                        else\
                                            DEB_TAB++;\
                                        for(int i = DEB_TAB; i > 0; i--)\
                                            printf(" ");\
                                        printf("%s\n", func);\
                                    }
static int DEBUG = 1;
static int DEB_TAB = 0;

static const char* S = NULL;
static const char* BEGIN = NULL;
static int CountError = 0;
static int Str_Num = 1;

Tree* get_G(const char* str);       // main
Node* get_F(Tree* AST);             // function (chaos, name ... thanatos)
Node* get_SEQ(Tree* AST);           // sequence of lines ... ! ...
Node* get_B(Tree* AST);             // block {...} or line
Node* get_L(Tree* AST);             // line: if, circle or assign
Node* get_C(Tree* AST);             // condition ||
Node* get_A(Tree* AST);             // condition and &&
Node* get_CMP(Tree* AST);           // > < >= <= ==
Node* get_E(Tree* AST);             // expression +-
Node* get_T(Tree* AST);             // next priority */
Node* get_D(Tree* AST);             // degree ^
Node* get_P(Tree* AST);             // primary expression (), N
Node* get_N(Tree* AST);             // number
Node* get_Arg(Tree* AST);           // arguments
Node* get_Id(Tree* AST);            // identifier
char* get_Name();

int Zulan_to_asm(const Tree* AST, const char* WRITENAME);
int node_to_asm(Node* node, const Tree* AST, FILE* asm_file);

#endif // ZULAN_H_INCLUDED
