// CPU . H

#ifndef CPU_H_INCLUDED
#define CPU_H_INCLUDED

#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <assert.h>
#include <string.h>
#include <math.h>
#include <ctype.h>
#include <windows.h>
#include <float.h>
#include "stack.h"
#include "text.h"
#include "Data.h"
#include "assembler.h"

#define ERRORTYPE -1
#define MISSMARK 1
#define NON_ARG 0
#define ARGUM 1
#define REGIST 2
#define ARR_ARG 3
#define ARR_REG 4
#define MARK 50

const int RAM_SIZE = 1000;
const int REGISTER_SIZE = 11;

typedef struct CPU CPU;

struct CPU
{
    cpu_type* Ram = NULL;
    cpu_type r[REGISTER_SIZE] = {};

    MyStack* func_calls = NULL;

    MyStack* data = NULL;

};

void cpu_Ctor(CPU* cpu);
void cpu_Dtor(CPU* cpu);
int cpu_get_commands(const char COMNAME[], const char DUMPNAME[], CPU* cpu);
int dump(const char DUMPNAME[], CPU* cpu, int command);

#endif // CPU_H_INCLUDED
