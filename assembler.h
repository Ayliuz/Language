// ASSEMBLER. H

#ifndef ASSEMBLER_H_INCLUDED
#define ASSEMBLER_H_INCLUDED

#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <assert.h>
#include <string.h>
#include <math.h>
#include <ctype.h>
#include "stack.h"
#include "text.h"
#include "Data.h"

#define PASS_CASE ;

#define ERRORTYPE -1
#define MISSMARK 1
#define NON_ARG 0
#define ARGUM 1
#define REGIST 2
#define ARR_ARG 3
#define ARR_REG 4
#define MARK 50
#define LOG 1
#define PROG 0

int assembler(const char READNAME[], const char COMNAME[]);

int funct_type (char* str, char* registr, char* arg);
char* skip_cmd_spaces_and_comment(char* str);
int command_cmp (const char* command, char* str);
int find_elem(char** arr, int N_arr, const char* str);
int ismark (char* str, char* *mark);
int get_mark (char* str, char* arg);
int put_commands(MyText* Program, char* assembl, char** marks);

#define STRLEN 50

int disassembler(const char COMNAME[], const char WRITENAME[], const int mode);

int get_commands(char* program, const size_t len, MyText* Code, int* marks);
int out_disasm(const char* name, const MyText* Code, const int* marks, const char* program, const int mode);

#endif // ASSEMBLER_H_INCLUDED
