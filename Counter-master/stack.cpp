// STACK. CPP

#include "stack.h"

//************************************
/// Compares double parameter with 0.
///
/// \param [in] double x
///
/// \return 1 if variable equals to 0 (less than 0.000001),
///         0 if variable is bigger than 0.000001
//************************************

int iszero(double x)
{
    return (fabs(x) < 0.000001 ) ? 1 : 0;
}

//************************************
/// Constructs MyStack structure and initialize it.
///
/// \param [in] struct MyStack* stack - pointer to MyStack structure
///
///
/// \return void
///
//************************************

void stack_Ctor(MyStack* stack)
{
    assert(stack);

    stack->stack_guard_begin = GUARD;
    stack->stack_guard_end = GUARD;

    stack->data = (stack_type*) calloc(StartCapacity + 2, sizeof(stack_type));
    stack->StackSize = 0;
    stack->StackCapacity = StartCapacity;

    set_guards(stack);

    for (unsigned int i = 0; i < (stack->StackCapacity); i++)
    {
        stack->data[i] = NAN;
    }

    make_hash(stack);
}

//************************************
/// Sets guards of the stack'stack array
///
/// \param [in] struct MyStack* stack - pointer to MyStack structure
///
///
/// \return void
///
//************************************

void set_guards(MyStack* stack)
{
    assert(stack);

    stack->data_guard_begin = stack->data;
    *(stack->data_guard_begin) = GUARD;

    stack->data = stack->data + 1;
    stack->data_guard_end = stack->data + stack->StackCapacity;
    *(stack->data_guard_end) = GUARD;
}

//************************************
/// Deletes MyStack structure and its data.
///
/// \param [in] struct MyStack* stack - pointer to MyStack structure
///
///
/// \return void
///
//************************************

void stack_Dtor(MyStack* stack)
{
    assert(stack);

    for (;stack->StackSize >= 1; stack->StackSize --)
    {
        stack->data[stack->StackSize - 1] = NAN;
    }

    free(stack->data);
    stack->data = NULL;

    stack->StackSize = SIZEPOIS;
    stack->StackCapacity = SIZEPOIS;

    stack->StackHash_data = HASHPOIS;
    stack->StackHash_stack = HASHPOIS;

    *(stack->data_guard_begin) = NAN;
    *(stack->data_guard_end) = NAN;
    stack->data_guard_begin = NULL;
    stack->data_guard_end = NULL;

    stack->stack_guard_begin = GUARDPOIS;
    stack->stack_guard_end = GUARDPOIS;

    stack = NULL;
}

//************************************
/// Clears MyStack structure'stack data.
///
/// \param [in] struct MyStack* stack - pointer to MyStack structure
///
///
/// \return void
///
//************************************


void stack_Clear(MyStack* stack)
{
    assert(stack);

    for (;stack->StackSize >= 1; stack->StackSize --)
    {
        stack->data[stack->StackSize - 1] = NAN;
    }

    stack->data = (stack_type*) realloc(stack->data, (StartCapacity + 2) * sizeof(stack_type));
    stack->StackCapacity = StartCapacity;

    set_guards(stack);

    make_hash(stack);
}

//************************************
/// Inserts an element on the top of stack
///
/// \param [in] struct MyStack* stack - pointer to MyStack structure
///
///
/// \return gives info about stack in console if there are any troubles
///
//************************************

void stack_push(MyStack* stack, stack_type val)
{
    assert(stack);
    if ( !stack_is_OK(stack)) {DUMP(StackErNum,stack); abort();}

    if (stack->StackSize + 1 < stack->StackCapacity)
    {
        stack->data[stack->StackSize] = val;
        stack->StackSize ++;
    }
    else
    {
        stack_extend(stack);
        stack->data[stack->StackSize] = val;
        stack->StackSize ++;
    }

    make_hash(stack);

    if ( !stack_is_OK(stack)) {DUMP(StackErNum,stack); abort();}
}

//************************************
/// Takes one element from the top of stack
///
/// \param [in] struct MyStack* stack - pointer to MyStack structure
///
///
/// \return stack_type element from the top of the stack, gives info about stack in console if there are any troubles
///
//************************************

stack_type stack_pop(MyStack* stack)
{
    assert(stack);
    if(stack->StackSize == 0)
    {
        StackErNum = ERRORPOP;
        DUMP(StackErNum, stack)
        return NAN;
    }

    if ( !stack_is_OK(stack)) {DUMP(StackErNum,stack); abort();}

    stack_type buf = stack->data[--(stack->StackSize)];
    stack->data[(stack->StackSize)] = NAN;

    if (((stack->StackCapacity) >= 4 * (stack->StackSize)) && (stack->StackCapacity > StartCapacity))
    {
        stack_contract(stack);
    }

    make_hash(stack);

    if ( !stack_is_OK(stack)) {DUMP(StackErNum,stack); abort();}
    return buf;

}

//************************************
/// Extends the array in the stack.
///
/// \param [in] struct MyStack* stack - pointer to MyStack structure
///
/// \return changes the size of arrow in stack.
///
//************************************

void stack_extend (MyStack* stack)
{
    assert(stack);

    stack->data = (stack_type*) realloc(stack->data_guard_begin, ((stack->StackCapacity) * 2 + 2) * sizeof(stack_type));
    stack->StackCapacity *= 2;

    set_guards(stack);

    for (unsigned int i = (stack->StackSize); i < (stack->StackCapacity); i++)
    {
        stack->data[i] = NAN;
    }
}

//************************************
/// Contracts the array in the stack.
///
/// \param [in] struct MyStack* stack - pointer to MyStack structure
///
/// \return changes the size of arrow in stack.
///
//************************************

void stack_contract (MyStack* stack)
{
    assert(stack);

    stack->data = (stack_type*) realloc(stack->data_guard_begin, (stack->StackCapacity / 2 + 2) * sizeof(stack_type) );
    stack->StackCapacity /= 2;

    set_guards(stack);
}

//************************************
/// Hashes the  stack.
///
/// \param [in] struct MyStack* stack - pointer to MyStack structure
///
/// \return changes StackHash_stack and StackHash_data in stack.
///
//************************************

void make_hash (MyStack* stack)
{
    assert(stack);

    stack->StackHash_data = hash (stack->data, stack->StackCapacity * sizeof (stack_type));
    stack->StackHash_stack = HASHDEFAULT;
    stack->StackHash_stack = hash (stack, sizeof (*stack));
}

//************************************
/// Hashes memory pointed by given pointer.
///
/// \param [in] struct MyStack* stack - pointer to MyStack structure
///
/// \return changes StackHash_data in stack.
///
//************************************

int hash (void* stack, unsigned int size)
{
    assert(stack);

    int hashSum = 0;
    for (char* p = (char*) stack; p < (char*)(stack + size); p++)
    {
        hashSum = 22222 * hashSum  + abs(int(*p));
        hashSum = hashSum % 20002;
    }

    return hashSum;
}


//************************************
/// Checks if stack is fine or not, and writes error code in global variable
///
/// \param [in] struct MyStack* stack - pointer to MyStack structure
///
/// \return 1 if stack is fine 0 if not, changes the global variable StackErNum (0 - all is right, ERRORDATA - data in stack is damaged, ERRORCAPACITY - not suitable stack capacity, \
ERRORSIZE - not suitable stack size, ERRORMEMORY - memory of stack data was damaged, ERRORSTRUCT - memory of stack struct was damaged)
///
//************************************

int stack_is_OK(MyStack* stack)
{
    assert(stack);
    int not_error = 1;

    if ( !(stack->data)){ StackErNum = ERRORDATA; return 0;}

    for (unsigned int i = stack->StackSize + 1; i < stack->StackCapacity; i++ )
    {
        if ( isfinite(stack->data[i]))
        {
            StackErNum = ERRORMEMORY;
            not_error = 0;
            break;
        }
    }

    if ( !iszero(hash(stack->data, sizeof(stack_type) * (stack->StackCapacity)) - (stack->StackHash_data))){                                                                        StackErNum = ERRORDATA;      not_error = 0; }

    if ( !iszero(*(stack->data_guard_begin) - GUARD) || !iszero(*(stack->data_guard_end) - GUARD)){                                                                             StackErNum = ERRORMEMORY;    not_error = 0;}

    int StackHashBuf = stack->StackHash_stack;
    stack->StackHash_stack = HASHDEFAULT;
    if ( !iszero(hash(stack, sizeof(*stack)) - StackHashBuf) || ((stack->stack_guard_begin) != GUARD) || ((stack->stack_guard_end != GUARD))){                                          StackErNum = ERRORSTRUCT;    not_error = 0;}
    stack->StackHash_stack = StackHashBuf;

    if ( !isfinite(stack->StackCapacity) || ((stack->StackCapacity) < StartCapacity) || (((stack->StackCapacity) % StartCapacity) != 0) || ((stack->StackSize) > (stack->StackCapacity))){  StackErNum = ERRORCAPACITY;  not_error = 0;}

    if ( !isfinite(stack->StackSize) || (((stack->StackSize) > (stack->StackCapacity)) && !isfinite(stack->data[stack->StackSize-1])) || isfinite(stack->data[stack->StackSize])) {                 StackErNum = ERRORSIZE;      not_error = 0;}

    return not_error;
}

//************************************
/// Tests stack_is_OK in case of StackSize
///
/// \param [in] struct MyStack* stack - pointer to MyStack structure
///
/// \return 1 if test was passed, 0 if not and calls for DUMP
///
//************************************

int test_error_size(const MyStack* stak)
{
    assert(stak);
    MyStack stack = *stak;
    stack_Ctor(&stack);

    for(double i = 2.01; i < 44.01; i = i + 1.0)
    {
        stack_push((&stack), 5.023 + i);
    }
    //
    stack.StackSize = 20;
        UNERTEST(ERRORSIZE, &stack)
    StackErNum = 0;
    //
    stack.StackSize = 150;
        UNERTEST(ERRORSIZE, &stack)
    StackErNum = 0;
    //
    stack.StackSize = NAN;
        UNERTEST(ERRORSIZE, &stack)
    StackErNum = 0;

    stack_Dtor(&stack);

    return 1;
}

//************************************
/// Tests stack_is_OK in case of StackCapacity
///
/// \param [in] struct MyStack* stack - pointer to MyStack structure
///
/// \return 1 if test was passed, 0 if not and calls for DUMP
///
//************************************

int test_error_capacity(const MyStack* stak)
{
    assert(stak);
    MyStack stack = *stak;
    stack_Ctor(&stack);

    for(double i = 2.01; i < 44.01; i = i + 1.0)
    {
        stack_push((&stack), 5.023 + i);
    }

    stack.StackCapacity = 10;
        UNERTEST(ERRORCAPACITY, &stack)
    StackErNum = 0;
    //
    stack.StackCapacity = 77;
        UNERTEST(ERRORCAPACITY, &stack)
    StackErNum = 0;
    //
    stack.StackCapacity = NAN;
        UNERTEST(ERRORCAPACITY, &stack)
    StackErNum = 0;

    stack_Dtor(&stack);

    return 1;
}

//************************************
/// Tests stack_is_OK in case of data
///
/// \param [in] struct MyStack* stack - pointer to MyStack structure
///
/// \return 1 if test was passed, 0 if not and calls for DUMP
///
//************************************

int test_error_data(const MyStack* stak)
{
    assert(stak);
    MyStack stack = *stak;
    stack_Ctor(&stack);


    for(double i = 2.01; i < 44.01; i = i + 1.0)
    {
        stack_push((&stack), 5.023 + i);
    }

    stack_type buf = stack.data[41];
    stack.data[41] = 90;
        UNERTEST(ERRORDATA, &stack)
    StackErNum = 0;
    stack.data[41] = buf;

    buf = stack.data[1];
    stack.data[1] = 92020;
        UNERTEST(ERRORDATA, &stack)
    StackErNum = 0;
    stack.data[1] = buf;

    buf = stack.data[20];
    stack.data[20] = 12340;
        UNERTEST(ERRORDATA, &stack)
    StackErNum = 0;
    stack.data[20] = buf;

    void* buf_point = stack.data;
    stack.data = NULL;
        UNERTEST(ERRORDATA, &stack)
    StackErNum = 0;
    stack.data = (stack_type*) buf_point;

    stack_Dtor(&stack);

    return 1;
}

//************************************
/// Tests stack_is_OK in case of data memory
///
/// \param [in] struct MyStack* stack - pointer to MyStack structure
///
/// \return 1 if test was passed, 0 if not and calls for DUMP
///
//************************************

int test_error_memory(const MyStack* stak)
{
    assert(stak);
    MyStack stack = *stak;
    stack_Ctor(&stack);


    for(double i = 2.01; i < 44.01; i = i + 1.0)
    {
        stack_push((&stack), 5.023 + i);
    }

    *(stack.data_guard_begin) = 500;
        UNERTEST(ERRORMEMORY, &stack)
    StackErNum = 0;
    *(stack.data_guard_begin) = GUARD;

    *(stack.data_guard_end) = -2200;
        UNERTEST(ERRORMEMORY, &stack)
    StackErNum = 0;
    *(stack.data_guard_end) = GUARD;

    stack_Dtor(&stack);

    return 1;
}

//************************************
/// Tests stack_is_OK in case of struct memory
///
/// \param [in] struct MyStack* stack - pointer to MyStack structure
///
/// \return 1 if test was passed, 0 if not and calls for DUMP
///
//************************************

int test_error_stackbuf(const MyStack* stak)
{
    assert(stak);
    MyStack stack = *stak;
    stack_Ctor(&stack);

    for(double i = 2.01; i < 44.01; i = i + 1.0)
    {
        stack_push((&stack), 5.023 + i);
    }

    void* buf_point = stack.data;
    stack.data += 3;
        UNERTEST(ERRORSTRUCT, &stack)
    StackErNum = 0;
    stack.data = (stack_type*) buf_point;

    buf_point = stack.data_guard_begin;
    stack.data_guard_begin += 5;
        UNERTEST(ERRORSTRUCT, &stack)
    StackErNum = 0;
    stack.data_guard_begin = (stack_type*) buf_point;

    buf_point = stack.data_guard_end;
    stack.data_guard_end -= 50;
        UNERTEST(ERRORSTRUCT, &stack)
    StackErNum = 0;
    stack.data_guard_end = (stack_type*) buf_point;

    stack.stack_guard_begin = 432200;
        UNERTEST(ERRORSTRUCT, &stack)
    StackErNum = 0;
    stack.stack_guard_begin = GUARD;

    stack.stack_guard_end = -4352328;
        UNERTEST(ERRORSTRUCT, &stack)
    StackErNum = 0;
    stack.stack_guard_end = GUARD;

    stack_Dtor(&stack);

    return 1;
}

//************************************
/// Tests stack
///
/// \param [in] struct MyStack* stack - pointer to MyStack structure
///
/// \return 1 if test was passed, 0 if not and calls for DUMP
///
//************************************

int test_stack(MyStack* stak)
{
    assert(stak);

    //
    if ( !test_error_data(stak))        return 0;
    //
    if ( !test_error_capacity(stak))    return 0;
    //
    if ( !test_error_size(stak))        return 0;
    //
    if ( !test_error_memory(stak))      return 0;
    //
    if ( !test_error_stackbuf(stak))    return 0;
    //
    //
    //Test error popping
    int counter = 0;
    for(double i = 2.01; i < 1225; i = i + 1.111, counter++)
    {
        printf("Testing stack: %.*s %.*s\r", counter * 55 / 2200, "||||||||||||||||||||||||||||||||||||||||||||||||||||||||", (2200 - counter) * 55 / 2200, "--------------------------------------------------------");
        stack_push((stak), 5.023 + i);
    }

    for(double i = 1224.11; i >= 2.01; i = i - 1.111, counter++)
    {
        printf("Testing stack: %.*s %.*s\r", counter * 55 / 2200, "||||||||||||||||||||||||||||||||||||||||||||||||||||||||", (2200 - counter) * 55 / 2200, "--------------------------------------------------------");
        UTEST(stack_pop(stak),(5.023 + i))
    }
    printf("\n");
    return 1;
}
