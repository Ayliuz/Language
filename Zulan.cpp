// ZULAN.CPP

#include "Zulan.h"

#define COUNT_ERROR(written_node)           if(CountError)\
                                            {\
                                                tree_erase(AST,written_node);\
                                                return NULL;\
                                            }

#define CLEAR_RESULT                        tree_erase(AST, result)

#define CLEAR_TREE                          CLEAR_RESULT;\
                                            tree_Dtor(AST);\
                                            free(AST);\
                                            free(code);

#define SKIPSPACES                          while(isspace(*S))\
                                            {\
                                                if(*S == '\n')\
                                                {\
                                                    Str_Num++;\
                                                    BEGIN = S;\
                                                }\
                                                S++;\
                                            };

#define COMMAND(str)                        !strncmp(S, str, strlen(str))

#define OPER_RESULT_CREATE(oper,val2)       operate_branch_create(&(AST->ver_num), oper, result, val2);

#define CORRECT_CHAR_CHECK(S)               (*S == '_' || *S == '$')

#define CLEAR_VAR_MEM(name_buf)             memset(name_buf, '\0', strlen(name_buf));\
                                            free(name_buf);

#define SET_RESULT(type)                    result = node_create(type, 0);\
                                            AST->ver_num++

#define RESULT_CHILD(child, side)           result->side = child;\
                                            SET_PARENT(child, result);

Tree* get_G(const char* str)
{
    assert(str);

    char* code = strdup(str);

    for(char* p = code; p < code + strlen(code); p++)
    {
        if (*p == ';')
        {
            while(*p != '\n')
                *(p++) = ' ';
        }
    }

    Tree* AST = (Tree*) calloc(1, sizeof(*AST));
    tree_Ctor(AST);

    S = code;
    BEGIN = code;
    CountError = 0;

    Node* result = get_F(AST);
    COUNT_ERROR(result);

    SKIPSPACES;
    SynAssert(G, *S == '\0', CLEAR_TREE);
    SynAssert(G, S != code, CLEAR_TREE);

    tree_set_root(AST, result);
    free(code);

    return AST;
}

Node* get_F(Tree* AST)
{
    assert(AST);

    DEBUG(F);

    const char* old_s = S;
    Node* result = node_create(INIT, 0);
    AST->ver_num++;

    SKIPSPACES;
    Node* current = result;
    while (COMMAND("chaos,"))
    {
        S += 6;
        SKIPSPACES;

        char* func_name = get_Name();
        COUNT_ERROR(result);

        current = tree_insert_right(AST, current, FUNC, 0);
        Node* body = NULL;
        AST->ver_num++;

        int func_index = find_elem(AST->functions, AST->func_count, func_name);
        if (func_index >= 0)
        {
            CLEAR_VAR_MEM(func_name);
            body = node_create(BODY, func_index);
        }
        else
        {
            AST->functions[AST->func_count++] = func_name;                                  // put new function to array of names

            SynAssert(F, AST->func_count < START_VAR_NUM, CLEAR_VAR_MEM(func_name));

            body = node_create(BODY, AST->func_count - 1);
        }

        LEFT(current) = body;
        SET_PARENT(body, current);

        SKIPSPACES;
        SynAssert(F, *S == '(', CLEAR_RESULT);
        S++;

        SKIPSPACES;
        if (*S != ')')
        {
            Node* arg = get_Arg(AST);
            COUNT_ERROR(result);

            RIGHT(body) = arg;
            SET_PARENT(arg, body);
        }

        SKIPSPACES;
        SynAssert(F, *S == ')', CLEAR_RESULT);
        S++;

        Node* block = get_SEQ(AST);
        COUNT_ERROR(result);

        LEFT(body) = block;
        SET_PARENT(block, body);

        SKIPSPACES;
        SynAssert(F, COMMAND("thanatos"), CLEAR_RESULT);
        S += 8;
        SKIPSPACES;
    }

    SynAssert(F, S != old_s, CLEAR_RESULT);

    DEBUG(\\F);
    return result;
}

Node* get_SEQ(Tree* AST)
{
    assert(AST);

    DEBUG(SEQ);

    const char* old_s = S;
    Node* result = get_B(AST);
    COUNT_ERROR(result);

    SKIPSPACES;
    Node* current = result;
    while (COMMAND("!"))
    {
        S++;

        SKIPSPACES;
        if(COMMAND("thanatos"))
        {
            break;
        }

        Node* new_cur = get_B(AST);
        COUNT_ERROR(result);

        RIGHT(current) = new_cur;
        SET_PARENT(new_cur, current);

        current = new_cur;

        SKIPSPACES;
    }

    SynAssert(SEQ, S != old_s, CLEAR_RESULT);

    DEBUG(\\SEQ);
    return result;
}

Node* get_B(Tree* AST)
{
    assert(AST);

    DEBUG(B);

    const char* old_s = S;
    Node* result = NULL;

    SKIPSPACES;
    if (COMMAND("chaos"))
    {
        S += 5;

        SET_RESULT(BLOCK);

        Node* lines = get_SEQ(AST);
        COUNT_ERROR(result);

        RESULT_CHILD(lines, left);

        SKIPSPACES;
        SynAssert(Bl, COMMAND("thanatos"), CLEAR_RESULT);
        S += 8;
    }
    else
    {
        SET_RESULT(LINE);

        Node* line = get_L(AST);
        COUNT_ERROR(result);

        RESULT_CHILD(line, left);
    }

    SynAssert(B, S != old_s, CLEAR_RESULT);

    DEBUG(\\B);
    return result;
}

Node* get_L(Tree* AST)
{
    assert(AST);

    DEBUG(L);

    const char* old_s = S;
    Node* result = NULL;

    SKIPSPACES;
    if (COMMAND("Tantalus"))
    {
        S += 8;

        SET_RESULT(IF);

        SKIPSPACES;
        SynAssert(L, *S == '(', CLEAR_RESULT);
        S++;

        Node* condition = get_C(AST);
        COUNT_ERROR(result);

        RESULT_CHILD(condition, left);

        SKIPSPACES;
        SynAssert(L, *S == ')', CLEAR_RESULT);
        S++;

        Node* action = get_B(AST);
        COUNT_ERROR(result);

        RESULT_CHILD(action, right);

    }
    else if (COMMAND("Heracles"))
    {
        S += 8;

        SET_RESULT(CIRCLE);

        SKIPSPACES;
        SynAssert(L, *S == '(', CLEAR_RESULT);
        S++;

        Node* condition = get_C(AST);
        COUNT_ERROR(result);

        SKIPSPACES;
        SynAssert(L, *S == ')', CLEAR_RESULT);
        S++;

        RESULT_CHILD(condition, left);

        Node* action = get_B(AST);
        COUNT_ERROR(result);

        RESULT_CHILD(action, right);
    }
    else
    {
        Node* id = get_Id(AST);
        COUNT_ERROR(id);

        SKIPSPACES;
        if (*S == '=' && id->type == VAR)
        {
            S++;
            SET_RESULT(ASSIGN);

            RESULT_CHILD(id, left);

            SKIPSPACES;
            Node* expr = get_E(AST);
            COUNT_ERROR(result);

            RESULT_CHILD(expr, right);
        }
    }

    SynAssert(L, S != old_s, CLEAR_RESULT);

    DEBUG(\\L);
    return result;
}

Node* get_C(Tree* AST)
{
    assert(AST);

    DEBUG(C);

    const char* old_s = S;

    Node* result = get_A(AST);
    COUNT_ERROR(result);

    SKIPSPACES;
    while (*S == '|' && *(S + 1) == '|')
    {
        S += 2;

        Node* new_expr = get_A(AST);
        COUNT_ERROR(result);

        result = OPER_RESULT_CREATE(OR, new_expr);

        SKIPSPACES;
    }

    SynAssert(C, S != old_s, CLEAR_RESULT);

    DEBUG(\\C);
    return result;
}

Node* get_A(Tree* AST)
{
    assert(AST);

    DEBUG(A);

    const char* old_s = S;

    Node* result = get_CMP(AST);
    COUNT_ERROR(result);

    SKIPSPACES;
    while (*S == '&' && *(S + 1) == '&')
    {
        S += 2;

        Node* cmp = get_CMP(AST);
        COUNT_ERROR(result);

        result = OPER_RESULT_CREATE(AND, cmp);

        SKIPSPACES;
    }

    SynAssert(A, S != old_s, CLEAR_RESULT);

    DEBUG(\\A);
    return result;
}

Node* get_CMP(Tree* AST)
{
    assert(AST);

    DEBUG(CMP);

    const char* old_s = S;
    Node* result = get_E(AST);
    COUNT_ERROR(result);

    SKIPSPACES;
    if (*S == '>' || *S == '<' || (*S == '=' && *(S + 1) == '='))
    {
        int op = 0;

        if(*S == '>')
        {
            S++;
            op = LAR;
        }
        else if(*S == '<')
        {
            S++;
            op = LESS;
        }
        else
        {
            S += 2;
            op = EQ;
        }

        Node* right_expr = get_E(AST);
        COUNT_ERROR(result);

        result = OPER_RESULT_CREATE(op, right_expr);
    }

    SynAssert(CMP, S != old_s, CLEAR_RESULT);

    DEBUG(\\CMP);
    return result;
}

Node* get_E(Tree* AST)
{
    assert(AST);

    DEBUG(E);

    const char* old_s = S;

    Node* result = get_T(AST);
    COUNT_ERROR(result);

    SKIPSPACES;
    while (*S == '+' || *S == '-')
    {
        int op = (*S == '+') ? SUM: SUB;
        S++;

        Node* summand = get_T(AST);
        COUNT_ERROR(result);

        result = OPER_RESULT_CREATE(op, summand);

        SKIPSPACES;
    }
    SynAssert(E, S != old_s, CLEAR_RESULT);

    AST_simplify(AST, result);
    DEBUG(\\E);
    return result;
}

Node* get_T(Tree* AST)
{
    assert(AST);

    DEBUG(T);

    const char* old_s = S;

    Node* result = get_D(AST);
    COUNT_ERROR(result);

    SKIPSPACES;
    while (*S == '*' || *S == '/')
    {
        int op = (*S == '*') ? MUL: DIV;
        S++;

        Node* mul = get_D(AST);
        COUNT_ERROR(result);

        result = OPER_RESULT_CREATE(op, mul);

        SKIPSPACES;
    }
    SynAssert(T, S != old_s, CLEAR_RESULT);

    DEBUG(\\T);
    return result;
}

Node* get_D(Tree* AST)
{
    assert(AST);

    DEBUG(D);

    const char* old_s = S;

    Node* result = get_P(AST);
    COUNT_ERROR(result);

    SKIPSPACES;
    if (*S == '^')
    {
        S++;
        Node* power = get_P(AST);
        COUNT_ERROR(power);

        result = OPER_RESULT_CREATE(POW, power);
    }
    SynAssert(D, S != old_s, CLEAR_RESULT);

    DEBUG(\\D);
    return result;
}

Node* get_P(Tree* AST)
{
    assert(AST);

    DEBUG(P);

    SKIPSPACES;
    if (*S == '(')
    {
        S++;
        Node* result = get_E(AST);
        COUNT_ERROR(result);

        SKIPSPACES;
        SynAssert(P, *S == ')', CLEAR_RESULT);
        S++;

        DEBUG(\\P);
        return result;
    }
    else if(isdigit(*S) || (*S == '-' && isdigit(*(S+1))))
    {
        DEBUG(\\P);
        return get_N(AST);
    }
    else
    {
        DEBUG(\\P);
        return get_Id(AST);
    }
}

Node* get_N(Tree* AST)
{
    assert(AST);

    DEBUG(N);

    const char* old_s = S;

    char* end_s = NULL;

    tree_type val = strtod(S, &end_s);
    S = end_s;
    SynAssert(N, S != old_s,;);

    AST->ver_num++;

    DEBUG(\\N);
    return node_create(CONST_T, val);
}

Node* get_Arg(Tree* AST)
{
    assert(AST);

    DEBUG(Arg);

    const char* old_s = S;

    Node* result = get_E(AST);
    COUNT_ERROR(result);

    SKIPSPACES;
    while (*S == ',')
    {
        S++;

        Node* new_arg = get_E(AST);
        COUNT_ERROR(result);

        RIGHT(result) = new_arg;
        SET_PARENT(new_arg, result);

        SKIPSPACES;
    }

    SynAssert(Arg, S != old_s, CLEAR_RESULT);

    DEBUG(\\Arg);
    return result;
}

Node* get_Id(Tree* AST)
{
    assert(AST);

    DEBUG(Id);

    char* buffer = get_Name();
    COUNT_ERROR(NULL);

    #define OP_DEF(operator, priority, read_body, count_body, diff_body, print_body, simplify_body, asm_body)\
                if(!strcmp(buffer, read_body))\
                {\
                        CLEAR_VAR_MEM(buffer);\
                        Node* result = get_P(AST);\
                        COUNT_ERROR(result);\
                        \
                        DEBUG(\Id);\
                        return operate_branch_create(&(AST->ver_num), operator, NULL, result);\
                }

    #include "Operators.h"

    #undef OP_DEF

    AST->ver_num++;

    SKIPSPACES;
    if(*S == '(')
    {
        S++;

        Node* result = NULL;
        SET_RESULT(CALL);

        int func_index = find_elem(AST->functions, AST->func_count, buffer);
        if (func_index >= 0)
        {
            CLEAR_VAR_MEM(buffer);
        }
        else
        {
            AST->functions[AST->func_count++] = buffer;                                  // put new function to array of names

            SynAssert(Id, AST->func_count < START_VAR_NUM, CLEAR_VAR_MEM(buffer));
        }

        SKIPSPACES;
        if (*S != ')')
        {
            Node* arg = get_Arg(AST);
            COUNT_ERROR(result);

            RESULT_CHILD(arg, left);
        }

        SKIPSPACES;
        SynAssert(Id, *S == ')', CLEAR_RESULT);
        S++;

        RESULT_CHILD(node_create(VAR, (func_index < 0)? AST->func_count - 1 : func_index), right);

        DEBUG(\\Id);
        return result;
    }
    else
    {
        int var_index = find_elem(AST->variables, AST->var_count, buffer);
        if (var_index >= 0)
        {
            CLEAR_VAR_MEM(buffer);

            DEBUG(\\Id);
            return node_create(VAR, var_index);
        }

        AST->variables[AST->var_count++] = buffer;                                  // put new variable to array of names

        SynAssert(Id, AST->var_count < START_VAR_NUM, CLEAR_VAR_MEM(buffer));

        DEBUG(\\Id);
        return node_create(VAR, AST->var_count - 1);
    }

    DEBUG(\\Id);
    return NULL;
}

char* get_Name()
{
    DEBUG(Name);

    char* name_buf = (char*) calloc(MAX_VAR_LEN, sizeof(*name_buf));

    const char* old_var_s = S;

    char* pos = name_buf;
    for (; ((isalpha(*S) || CORRECT_CHAR_CHECK(S))&& S == old_var_s) || ((isalnum(*S) || CORRECT_CHAR_CHECK(S)) && S != old_var_s ); pos++, S++)
    {
        SynAssert(GetName, pos - name_buf < MAX_VAR_LEN - 1, CLEAR_VAR_MEM(name_buf));
        *pos = *S;
    }
    *pos = '\0';

    SynAssert(GetName, S != old_var_s, CLEAR_VAR_MEM(name_buf));

    DEBUG(\\Name);
    return name_buf;
}

#define APRINT(arguments)           fprintf(asm_file, arguments)
#define APRINT_ARG(format, arg)     fprintf(asm_file, format, arg)

#define IND(node)                   ((int) node) % 10000

int Zulan_to_asm(const Tree* AST, const char* WRITENAME)
{
    assert(AST);

    FILE* asm_file = fopen(WRITENAME, "w");

    node_to_asm(ROOT(AST), AST, asm_file);

    APRINT("end\0");

    fclose(asm_file);
    return 0;
}

int node_to_asm(Node* node, const Tree* AST, FILE* asm_file)
{
    assert(AST && asm_file);

    if (!node)
    {
        return 0;
    }

    switch(node->type)
    {
        case INIT:
        {
            APRINT("push 0\n");
            APRINT("pop r1\n");
            node_to_asm(RIGHT(node), AST, asm_file);
            break;
        }
        case FUNC:
        {
            node_to_asm(LEFT(node), AST, asm_file);
            node_to_asm(RIGHT(node), AST, asm_file);
            break;
        }

        case BODY:
        {
            APRINT_ARG( "push %i\n", AST->var_count);
            APRINT(     "push r1\n");
            APRINT(     "add\n");
            APRINT(     "pop r1\n");

            int count_arg = 0;
            Node* arg_node = RIGHT(node);
            while(arg_node)
            {
                count_arg++;
                arg_node = RIGHT(arg_node);
                APRINT_ARG( "push %i\n", count_arg);
                APRINT(     "push r1\n");
                APRINT(     "add\n");
                APRINT(     "pop r3\n");
                APRINT(     "pop [r3]\n");
            }

            APRINT_ARG("\n: %s\n", AST->functions[(int) node->info]);

            node_to_asm(LEFT(node), AST, asm_file);
            break;
        }

        case BLOCK:
        {
            node_to_asm(LEFT(node), AST, asm_file);
            node_to_asm(RIGHT(node), AST, asm_file);
            break;
        }

        case LINE:
        {
            node_to_asm(LEFT(node), AST, asm_file);
            node_to_asm(RIGHT(node), AST, asm_file);
            break;
        }

        case IF:
        {
            node_to_asm(LEFT(node), AST, asm_file);
            APRINT(     "push 0\n");
            APRINT_ARG( "ifeq %i\n", IND(node));

            node_to_asm(RIGHT(node), AST, asm_file);

            APRINT_ARG( "\n: %i\n", IND(node));
            break;
        }

        case CIRCLE:
        {
            APRINT_ARG( "jump c%i\n", IND(node));
            node_to_asm(LEFT(node), AST, asm_file);
            APRINT(     "push 0\n");
            APRINT_ARG( "ifeq %i\n", IND(node));

            node_to_asm(RIGHT(node), AST, asm_file);

            APRINT_ARG( "jump c%i\n", IND(node));
            APRINT_ARG( "\n: %i\n", IND(node));
            break;
        }

        case ASSIGN:
        {
            node_to_asm(RIGHT(node), AST, asm_file);
            APRINT(     "push r1\n");
            APRINT_ARG( "push %i\n", (int) LEFT(node)->info);
            APRINT(     "add\n");
            APRINT(     "pop r3\n");
            APRINT(     "pop [r3]\n");
            break;
        }

        case CALL:
        {
            if (!strcmp(AST->variables[(int) RIGHT(node)->info], "prometheus"))
            {
                APRINT(     "push r1\n");
                APRINT_ARG( "push %i\n", AST->var_count);
                APRINT(     "subtr\n");
                APRINT(     "pop r1\n");
                APRINT("ret\n\n\n");
                break;
            }

            if (!strcmp(AST->variables[(int) RIGHT(node)->info], "orpheus"))
            {
                APRINT(     "in r5\n");
                APRINT(     "push r5\n");
                APRINT(     "push r1\n");
                APRINT_ARG( "push %i\n", (int) LEFT(node)->info);
                APRINT(     "add\n");
                APRINT(     "pop r3\n");
                APRINT(     "pop [r3]\n");
                break;
            }

            if (!strcmp(AST->variables[(int) RIGHT(node)->info], "hades"))
            {
                APRINT(     "push r1\n");
                APRINT_ARG( "push %i\n", (int) LEFT(node)->info);
                APRINT(     "add\n");
                APRINT(     "pop r3\n");
                APRINT(     "push [r3]\n");
                APRINT(     "out\n");
                break;
            }


            int count_arg = 0;
            Node* arg_node = LEFT(node);
            while(arg_node)
            {
                APRINT(     "push r1\n");
                APRINT_ARG( "push %i\n", (int) arg_node->info);
                APRINT(     "add\n");
                APRINT(     "pop r3\n");
                APRINT(     "push [r3]\n");
                arg_node = RIGHT(arg_node);
                count_arg++;
            }

            APRINT_ARG(     "call %s\n", AST->variables[(int) RIGHT(node)->info]);

            break;
        }

        case CONST_T:
        {
            APRINT_ARG(     "push %lf\n", node->info);
            break;
        }

        case VAR:
        {
            APRINT(     "push r1\n");
            APRINT_ARG( "push %i\n", (int) node->info);
            APRINT(     "add\n");
            APRINT(     "pop r3\n");
            APRINT(     "push [r3]\n");
            break;
        }

        case OP:
        {
            switch(OPER_VAL(node))
            {
                #define OP_DEF(operator, priority, read_body, count_body, diff_body, print_body, simplify_body, asm_body)\
                                    case (operator):\
                                    {\
                                        asm_body;\
                                        break;\
                                    }

                #include "Operators.h"

                #undef OP_DEF

                default:
                    return 1;
            }
            break;
        }

        default:
            return 1;
    }

    return 0;
}


