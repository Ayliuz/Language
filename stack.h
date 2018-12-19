// STACK. H

#ifndef STACK_H_INCLUDED
#define STACK_H_INCLUDED

#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <assert.h>
#include <string.h>
#include <math.h>
#include <ctype.h>

#define GUARD 32234
#define GUARDPOIS -1
#define SIZEPOIS -1
#define HASHPOIS -1
#define HASHDEFAULT 0
#define ERROROK 0
#define ERRORDATA 1
#define ERRORCAPACITY 2
#define ERRORSIZE 3
#define ERRORPOP 4
#define ERRORMEMORY 5
#define ERRORSTRUCT 6

#ifndef $
#define $(x) std::cout<<"~"<<#x " = "<<x<<"\n";
#endif

#define DUMP(ERRNUMBER,stack) { printf("~In File: %s\n~In Line: %d\n~In Function: %s\n", __FILE__, __LINE__, __FUNCTION__); \
                                printf("~stack " #stack " [0x%X]\n~{\n~   Capacity = %u\n~   Size = %u\n~   data [0x%X]:\n~",  (stack), (stack)->StackCapacity,\
                                (stack)->StackSize, (stack)->data);\
                                int bufn = (stack)->StackSize;\
                                for (int i = 0; i < bufn; i++)\
                                { \
                                    printf("      data[%d] = %f\n~", i, (stack)->data[i]);\
                                } \
                                printf("   In memory StackHash = %i\n~",          (stack)->StackHash_stack);\
                                int StackHashBuf = (stack)->StackHash_stack;\
                                (stack)->StackHash_stack = HASHDEFAULT;\
                                printf("       Real stack_hash = %i\n~",         hash ((stack), sizeof (*(stack))));\
                                (stack)->StackHash_stack = StackHashBuf;\
                                printf("   In memory DataHash  = %i\n~",           (stack)->StackHash_data);\
                                printf("       Real data_hash  = %i\n~",          hash((stack)->data, (stack)->StackCapacity * sizeof(stack_type)));\
                                printf("   Struct_guard_begin  = %s\n~", (((stack)->stack_guard_begin) == GUARD) ? "GUARD": "ERROR");\
                                printf("   Struct_guard_end    = %s\n~",   (((stack)->stack_guard_end) == GUARD) ? "GUARD": "ERROR");\
                                printf("   Data_guard_begin    = %s\n~", iszero(*((stack)->data_guard_begin) - GUARD) ? "GUARD": "ERROR");\
                                printf("   Data_guard_end      = %s\n~",   iszero(*((stack)->data_guard_end) - GUARD) ? "GUARD": "ERROR");\
                                printf("   INFO: \n~");\
                                switch (ERRNUMBER)\
                                { \
                                    case ERROROK:\
                                        printf("      Stack is OK\n~");\
                                        break;\
                                    case ERRORDATA:\
                                        printf("      Data or hash was damaged\n~");\
                                        break;\
                                    case ERRORCAPACITY:\
                                        printf("      Capacity is not suitable\n~");\
                                        break;\
                                    case ERRORSIZE:\
                                        printf("      Size is not suitable\n~");\
                                        break;\
                                    case ERRORPOP:\
                                        printf("      Can't pop from the stack\n~");\
                                        break;\
                                    case ERRORMEMORY:\
                                        printf("      Data memory was damaged\n~");\
                                        break;\
                                    case ERRORSTRUCT:\
                                        printf("      Struct elements of stack was damaged\n~");\
                                        break;\
                                    default:\
                                        printf("      Stack is OK\n~");\
                                }\
                                printf("}\n\n");\
                              }

#define UTEST(result,wanted) {\
                                    if ( !iszero(result - wanted))\
                                    {\
                                        printf("~Error in testing: in File: %s\n~In Line: %d\n~In Function: %s\n", __FILE__, __LINE__, __FUNCTION__);\
                                        printf("~Test result " #result " = %f is not correct ( != %f)", result, wanted);\
                                        StackErNum = ERRORPOP;\
                                        DUMP (StackErNum,stak)\
                                        StackErNum =0;\
                                        return 0;\
                                    }\
                            }
#define UNERTEST(ErrorName, stak){\
                                if ((stack_is_OK(stak) != 0) || (StackErNum != ErrorName))\
                                {\
                                    printf("~In %s, line %d, function: %s\n   ~Error in stack_is_OK(with error code " #ErrorName " = %d), error code = %d\n", __FILE__,  __LINE__, __FUNCTION__, ErrorName, StackErNum);\
                                    DUMP (StackErNum,(stak))\
                                    StackErNum = 0;\
                                    return 0;\
                                }\
                                \
                            }
typedef double stack_type;
typedef struct MyStack MyStack;

const unsigned int StartCapacity = 10;
static int StackErNum = ERROROK;

struct MyStack
{
    int stack_guard_begin = GUARDPOIS;

    stack_type* data = NULL;

    unsigned int StackSize = SIZEPOIS;
    unsigned int StackCapacity = SIZEPOIS;

    int StackHash_data = HASHDEFAULT;
    int StackHash_stack = HASHDEFAULT;

    stack_type* data_guard_begin = NULL;
    stack_type* data_guard_end = NULL;

    int stack_guard_end = GUARDPOIS;
};

int iszero(const double x);

void stack_Ctor(MyStack* stack);
void set_guards(MyStack* stack);
void stack_Dtor(MyStack* stack);

void stack_Clear(MyStack* stack);

void stack_push(MyStack* stack, stack_type val);
stack_type stack_pop(MyStack* stack);

void stack_extend (MyStack* stack);
void stack_contract (MyStack* stack);

void make_hash (MyStack* stack);
int hash (void* object, unsigned int size);

int stack_is_OK(MyStack* stack);

int test_stack(MyStack* stack);
int test_error_size(const MyStack* stack);
int test_error_capacity(const MyStack* stack);
int test_error_data(const MyStack* stack);
int test_error_memory(const MyStack* stack);
int test_error_stackbuf(const MyStack* stack);



#endif // STACK_H_INCLUDED
