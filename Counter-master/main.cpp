#include "D:\TX\TXlib.h"
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include "Counter.h"

const char dot[100] = "\"C:\\Program Files (x86)\\Graphviz2.38\\bin\\gvedit.exe\" -Tbmp ";

const char* DUMPNAME  = "Counter";
const char* SAVENAME  = "printCounter.txt";
const char* PRINTNAME = "output";

int main()
{
    txSetConsoleAttr (0xf0);

    char expression[1000] = "(3 - (cos(ln(2)))^3)^(1/2)";
    //scanf("%s", expression);

    Tree* Counter = get_G(expression);
    if (Counter)
    {
        AST_diff("x", Counter, ROOT(Counter));

        AST_simplify(Counter, ROOT(Counter));

        tree_save(Counter, SAVENAME);
        AST_print(PRINTNAME, Counter);

        tree_dump(dot, DUMPNAME, Counter);
        tree_Dtor(Counter);
    }


    free(Counter);
    return 0;
}
