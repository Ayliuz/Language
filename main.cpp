#include "D:\TX\TXlib.h"
#include <iostream>
#include <stdio.h>
#include <stdlib.h>

#include "Zulan.h"

const char dot[100] = "\"C:\\Program Files (x86)\\Graphviz2.38\\bin\\dot.exe\" -Tbmp ";

const char* DUMPNAME = "ZulanTree";
const char* CODENAME = "Factor.txt";
const char* CODETOASM = "Make.txt";

#define END cpu_Dtor(&cpu); return 1;

int main()
{
    txSetConsoleAttr (0xf0);

    char* code = read_from_file(CODENAME, READ);

    CPU cpu;
    cpu_Ctor(&cpu);

    Tree* AST = get_G(code);
    if (AST)
    {
        Zulan_to_asm(AST, CODETOASM);

        if (assembler(CODETOASM, COMNAME)) {                printf("~~~~~~~~~~~Assemble error~~~~~~~~~~~~~~~~~\n"); END; }
        if (disassembler(COMNAME, WRITENAME, LOG)) {        printf("~~~~~~~~~~~Disassemble error~~~~~~~~~~~~~~~~~\n"); END; }
        if (cpu_get_commands(COMNAME, STACK_DUMP, &cpu)) {  printf("~~~~~~~~~~~CPU error~~~~~~~~~~~~~~~~~\n"); END; }

        tree_Dtor(AST);
        free(AST);
    }

    cpu_Dtor(&cpu);
    return 0;
}
