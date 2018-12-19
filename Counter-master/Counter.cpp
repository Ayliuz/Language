// COUNTER.CPP

#include "Counter.h"

#define COUNT_ERROR(written_node)           if(CountError)\
                                            {\
                                                tree_erase(AST,written_node);\
                                                return NULL;\
                                            }

#define CLEAR_RESULT                        tree_erase(AST, result)

#define CLEAR_TREE                          CLEAR_RESULT;\
                                            tree_Dtor(AST);\
                                            free(AST);\

#define SKIPSPACES                          while(isspace(*S))\
                                            {\
                                                S++;\
                                            };\


Tree* get_G(const char* str)
{
    Tree* AST = (Tree*) calloc(1, sizeof(*AST));
    tree_Ctor(AST);

    S = str;
    BEGIN = str;
    CountError = 0;

    Node* result = get_E(AST);
    COUNT_ERROR(result);

    SKIPSPACES;
    SynAssert(G, *S == '\0', CLEAR_TREE);
    SynAssert(G, S != str, CLEAR_TREE);

    tree_set_root(AST, result);

    return AST;
}

#define OPER_RESULT_CREATE(oper,val2)              operate_branch_create(&(AST->ver_num), oper, result, val2);

Node* get_E(Tree* AST)
{
    assert(AST);

    const char* old_s = S;

    Node* result = get_T(AST);
    COUNT_ERROR(result);

    SKIPSPACES;
    while(*S == '+' || *S == '-')
    {
        int op = (*S == '+') ? SUM: SUB;
        S++;

        Node* summand = get_T(AST);
        COUNT_ERROR(result);

        result = OPER_RESULT_CREATE(op, summand);

        SKIPSPACES;
    }
    SynAssert(E, S != old_s, CLEAR_RESULT);

    return result;
}

Node* get_T(Tree* AST)
{
    assert(AST);

    const char* old_s = S;

    Node* result = get_D(AST);
    COUNT_ERROR(result);

    SKIPSPACES;
    while(*S == '*' || *S == '/')
    {
        int op = (*S == '*') ? MUL: DIV;
        S++;

        Node* mul = get_D(AST);
        COUNT_ERROR(result);

        result = OPER_RESULT_CREATE(op, mul);

        SKIPSPACES;
    }
    SynAssert(T, S != old_s, CLEAR_RESULT);

    return result;
}

Node* get_D(Tree* AST)
{
    assert(AST);

    const char* old_s = S;

    Node* result = get_P(AST);
    COUNT_ERROR(result);

    SKIPSPACES;
    if(*S == '^')
    {
        S++;
        Node* power = get_P(AST);
        COUNT_ERROR(power);

        result = OPER_RESULT_CREATE(POW, power);
    }
    SynAssert(D, S != old_s, CLEAR_RESULT);

    return result;
}

Node* get_P(Tree* AST)
{
    assert(AST);

    SKIPSPACES;
    if(*S == '(')
    {
        S++;
        Node* result = get_E(AST);
        COUNT_ERROR(result);

        SKIPSPACES;
        SynAssert(P, *S == ')', CLEAR_RESULT);
        S++;
        return result;
    }
    else if(isdigit(*S) || (*S == '-' && isdigit(*(S+1))))
    {
        return get_N(AST);
    }
    else
    {
        return get_Id(AST);
    }
}

Node* get_N(Tree* AST)
{
    assert(AST);

    const char* old_s = S;

    char* end_s = NULL;

    tree_type val = strtod(S, &end_s);
    S = end_s;
    SynAssert(N, S != old_s,;);

    AST->ver_num++;

    return node_create(CONST_T, val);
}

#define CORRECT_CHAR_CHECK(S)           (*S == '_' || *S == '$')

#define CLEAR_VAR_MEM                   memset(variable, '\0', strlen(variable));\
                                        free(variable);

Node* get_Id(Tree* AST)
{
    assert(AST);

    const char* old_s = S;

    char* variable = (char*) calloc(MAX_VAR_LEN, sizeof(*variable));

    for (char* pos = variable; ((isalpha(*S) || CORRECT_CHAR_CHECK(S))&& S == old_s) || ((isalnum(*S) || CORRECT_CHAR_CHECK(S)) && S != old_s ); pos++, S++)
    {
        SynAssert(Id, pos - variable < MAX_VAR_LEN, CLEAR_VAR_MEM);
        *pos = *S;
    }

    SynAssert(Id, S != old_s, CLEAR_VAR_MEM);

    #define OP_DEF(operator, priority, read_body, count_body, diff_body, print_body, simplify_body)\
                if(!strncmp(variable, read_body, strlen(variable)))\
                {\
                        Node* result = get_P(AST);\
                        COUNT_ERROR(result);\
                        \
                        return operate_branch_create(&(AST->ver_num), operator, NULL, result);\
                }

    #include "Operators.h"

    #undef OP_DEF

    AST->ver_num++;

    int var_index = find_elem(AST->variables, AST->var_count, variable);
    if (var_index >= 0)
    {
        CLEAR_VAR_MEM;
        return node_create(VAR, var_index);
    }

    AST->variables[AST->var_count++] = variable;                                  // put new variable to array of names

    SynAssert(Id, AST->var_count < START_VAR_NUM, CLEAR_VAR_MEM);

    return node_create(VAR, AST->var_count - 1);
}

Node* operate_branch_create(unsigned int *out_tree_ver_num, tree_type oper, Node* value1, Node* value2)
{
    assert(out_tree_ver_num);

    Node* new_branch = node_create(OP, oper);
    ++*out_tree_ver_num;

    SET_PARENT(value1, new_branch);
    SET_PARENT(value2, new_branch);

    SET_CHILDREN(new_branch, value1, value2);

    return new_branch;
}

tree_type AST_count(Tree* AST)
{
    assert(AST);

    tree_type value = leaf_counter(ROOT(AST));

    return value;
}

#define LEFT_COUNT              leaf_counter(LEFT(leaf))

#define RIGHT_COUNT             leaf_counter(RIGHT(leaf))


tree_type leaf_counter(Node* leaf)
{
    if(!leaf)
        return POISON;

    tree_type result = POISON;
    switch(leaf->type)
    {
        case OP:
            switch((int) leaf->info)
            {
                #define OP_DEF(operator, priority, read_body, count_body, diff_body, print_body, simplify_body)\
                                            case (operator):\
                                            {\
                                                count_body;\
                                                break;\
                                            }

                #include "Operators.h"

                #undef OP_DEF

                default:
                    return POISON;
            }
            break;

        case CONST_T:
            result = leaf->info;
            break;

        default:
            return POISON;
    }

    return result;
}

Node* AST_diff(char* variable, Tree* AST, Node* start_node)
{
    assert(AST);

    if (!start_node)
    {
        return NULL;
    }

    int var_index = find_elem(AST->variables, AST->var_count, variable);
    if (var_index < 0)
    {
        return NULL;
    }

    int diff_tree_size = 0;
    Node* Diff_node = leaf_diff(var_index, start_node, &diff_tree_size);

    tree_node_replace(AST, start_node, Diff_node);
    AST->ver_num += diff_tree_size;

    tree_make_hash(AST);

    return Diff_node;
}

#define NEW_NODE(node)              new_node = (node);

#define OPER_VAL(node)                  ((int) (node)->info)

Node* leaf_diff(int var_index, Node* node, int *out_vertex_num)
{
    assert(out_vertex_num);

    if(!node)
    {
        return NULL;
    }

    Node* new_node = NULL;
    unsigned int ver_num_increase = 0;                      // for branch_copy and operate_create, for lead_diff out_vertex_num is used

    switch(node->type)
    {
        case CONST_T:
            NEW_NODE(node_create(CONST_T, 0));
            ++ver_num_increase;
            break;

        case VAR:
            if(OPER_VAL(node) == var_index)
            {
                NEW_NODE(node_create(CONST_T, 1));
            }
            else
            {
                NEW_NODE(node_create(CONST_T, 0));
            }
            ++ver_num_increase;
            break;

        case OP:
            switch(OPER_VAL(node))
            {
                #define OP_DEF(operator, priority, read_body, count_body, diff_body, print_body, simplify_body)\
                                    case (operator):\
                                    {\
                                        diff_body;\
                                        break;\
                                    }

                #include "Operators.h"

                #undef OP_DEF

                default:
                    return NULL;
            }
            break;

        default:
            return NULL;
    }

    *out_vertex_num += ver_num_increase;
    return new_node;
}


Tree* AST_simplify(Tree* AST, Node* start_node)
{
    assert(AST);

    if (!start_node)
    {
        return NULL;
    }

    int simple_tree_size = 0;

    if(leaf_simplify(start_node, &simple_tree_size))
    {
        return NULL;
    }

    AST->ver_num += simple_tree_size;

    tree_make_hash(AST);
    return AST;
}


int leaf_simplify(Node* node, int* out_vertex_num)
{
    assert(out_vertex_num);

    if(!node)
    {
        return 0;
    }

    if(leaf_simplify(LEFT(node), out_vertex_num))
    {
        return 1;
    }

    if(leaf_simplify(RIGHT(node), out_vertex_num))
    {
        return 1;
    }


    switch(node->type)
    {
        case VAR:
        case CONST_T:
            return 0;

        case OP:
            switch(OPER_VAL(node))
            {
                #define OP_DEF(operator, priority, read_body, count_body, diff_body, print_body, simplify_body)\
                    case (operator):\
                        {\
                            simplify_body;\
                            break;\
                        }\

                #include "Operators.h"

                #undef OP_DEF

                default:
                    return 1;
            }
            break;

        default:
            return 1;

    }

    return 0;
}



#define TO_FORMAT(name,format)                  strdup(PRINTNAME); strcat(name, "."#format)

#define CONCATINATION(str,arg)                  strcat(str, arg)

void tex_begin(FILE* write_file)
{
    fprintf(write_file, "\\documentclass[12pt,a4paper]{scrartcl}\n\
            \\usepackage{indentfirst}\n\
            \\usepackage{amsmath}\n\
            \\begin{document}\n\
            $\n");
}

void tex_end(FILE* write_file)
{
    fprintf(write_file, "\n $ \n \\end{document}");
}

void tex_launch(const char* tex_name, const char* pdf_name)
{
    char command[200] = "pdflatex ";                                // make
    CONCATINATION(command, tex_name);                               // latex

    system(command);                                                //compile latex
    system(pdf_name);                                               //open latex
}

int AST_print(const char PRINTNAME[], Tree* AST)
{
    char* tex_name = TO_FORMAT(tex_name, tex);        // make tex file name
    char* pdf_name = TO_FORMAT(pdf_name, pdf);        // make pdf file name


    FILE* tex_file = fopen(tex_name, "w");
    if (!tex_file)
    {
        return 1;
    }

    tex_begin(tex_file);

    leaf_print(tex_file, AST, ROOT(AST));

    tex_end(tex_file);
    fclose(tex_file);

    tex_launch(tex_name, pdf_name);

    free(tex_name);
    free(pdf_name);

    return 0;
}

#define FPRINTF_ARG(format, state)  fprintf(print_file, format, state)

#define FPRINTF(state)              fprintf(print_file, state)

int leaf_print(FILE* print_file, Tree* AST, Node* node)
{
    if (!node)
    {
        return 0;
    }

    char dump_str[MAX_VAR_LEN + 100] = "";
    int priority = 0;                       // for brackets

    switch(node->type)
    {

        case OP:
            {
                if (PARENT(node) && (OPER_VAL(PARENT(node)) != DIV) && (operation_priority(OPER_VAL(node)) > operation_priority(OPER_VAL(PARENT(node)))))
                {
                    FPRINTF(" \\left( ");
                    priority = 1;
                }

                switch(OPER_VAL(node))
                {
                    #define OP_DEF(operator, priority, read_body, count_body, diff_body, print_body, simplify_body)\
                                    case (operator):\
                                    {\
                                        print_body;\
                                        break;\
                                    }

                    #include "Operators.h"

                    #undef OP_DEF

                    default:
                        return 0;

                }

                if (priority)
                {
                    FPRINTF(" \\right) ");
                }
                break;
            }

        case CONST_T:
            if (node->info < 0)
            {
                    FPRINTF(" \\left( ");
                    priority = 1;
            }

            FPRINTF_ARG("%lg", node->info);

            if (priority)
            {
                    FPRINTF(" \\right) ");
            }
            break;

        case VAR:
            FPRINTF_ARG("%s", AST->variables[(int) node->info]);
            break;

        default:
            return 0;

    }

    return 0;
}



int operation_priority(int operation)
{

    #define OP_DEF(operator, priority, read_body, count_body, diff_body, print_body, simplify_body)\
                if (operation == operator)\
                    {\
                        return priority;\
                    }

    #include "Operators.h"

    #undef OP_DEF
    return 0;
}
