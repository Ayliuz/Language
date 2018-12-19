//// TREE.CPP

#include "CountTree.h"


void tree_Ctor(Tree* tree)
{
    assert(tree);

    tree->tree_guard_begin = GUARD;
    tree->tree_guard_end = GUARD;

    ROOT(tree) = NULL;
    tree->variables = (char**) calloc(START_VAR_NUM, sizeof(*tree->variables));

    tree->ver_num = 0;
    tree->var_count = 0;

    tree_make_hash(tree);
}

void tree_Dtor(Tree* tree)
{
    assert(tree);

    tree_clear(tree);
    free_table(tree->variables, tree->var_count);

    tree->TreeHash_struct = HASHPOIS;

    tree->tree_guard_begin = GUARDPOIS;
    tree->tree_guard_end = GUARDPOIS;
}

void free_table (char** table, int n_table)
{
    assert(table);

    for (int i = 0; i < n_table; i++)
    {
        if (table[i])
        {
            memset (table[i], '\0', strlen(table[i]));
            free(table[i]);
            table[i] = NULL;
        }
    }
    free(*table);
}

void node_Ctor(Node* node)
{
    assert(node);

    node->node_guard_begin = GUARD;
    node->node_guard_end = GUARD;

    PARENT(node) = NULL;
    LEFT(node) = NULL;
    RIGHT(node)= NULL;

    node->info = POISON;
    node->type = NO_TYPE;
}

void node_Dtor(Node* node)
{
    assert(node);

    node->node_guard_begin = GUARDPOIS;
    node->node_guard_end = GUARDPOIS;

    PARENT(node) = NULL;
    LEFT(node) = NULL;
    RIGHT(node)= NULL;

    node->info = POISON;
    node->type = NO_TYPE;
}

void tree_make_hash(Tree* tree)
{
    assert(tree);

    tree->TreeHash_struct = HASHDEFAULT;
    tree->TreeHash_struct = hash (tree, sizeof (*tree));
}

Node* node_create(int type, tree_type val)
{
    if (type == NO_TYPE)
    {
        return NULL;
    }

    Node* new_node = (Node*) calloc(1, sizeof(*new_node));
    node_Ctor(new_node);
    new_node->info = val;
    new_node->type = type;

    return new_node;
}

Node* branch_copy(Node* source_node, unsigned int* out_vertex_num)
{
    assert(out_vertex_num);

    if (!source_node)
    {
        return NULL;
    }

    Node* new_node = node_create(source_node->type, source_node->info);

    LEFT(new_node)  = branch_copy(LEFT(source_node), out_vertex_num);
    RIGHT(new_node) = branch_copy(RIGHT(source_node), out_vertex_num);

    SET_PARENT(LEFT(new_node), new_node);
    SET_PARENT(RIGHT(new_node), new_node);

    ++*out_vertex_num;

    return new_node;
}

Node* tree_set_root(Tree* tree, Node* root)
{
    if (!root)
    {
        return ROOT(tree);
    }

    if (ROOT(tree))
    {
        PARENT(root) = PARENT(ROOT(tree));
        tree_erase(tree, ROOT(tree));
        ROOT(tree) = root;
    }
    else
    {
        ROOT(tree) = root;
        if (!tree->ver_num)
            tree->ver_num++;
    }

    tree_make_hash(tree);

    return ROOT(tree);
}

int node_swap(Node* node1, Node* node2)
{
    if (!node1 || !node2)
    {
        return 0;
    }

    tree_type temp_val = node1->info;
    int temp_type = node1->type;

    node1->type = node2->type;
    node1->info = node2->info;

    node2->type = temp_type;
    node2->info = temp_val;

    return 0;
}

#define INSERT(pos) \
                                if (parent_node->pos)\
                                {\
                                    parent_node->pos->info = new_val;\
                                    parent_node->pos->type = type;\
                                    return parent_node->pos;\
                                }\
                                Node* new_node = node_create(type, new_val);\
                                PARENT(new_node) = parent_node;\
                                parent_node->pos = new_node;\
                                tree->ver_num++;\
                                tree_make_hash(tree);

Node* tree_insert_left(Tree* tree, Node* parent_node, int type, tree_type new_val)
{
    assert(tree && parent_node);

    INSERT(left);

    return new_node;
}

Node* tree_insert_right(Tree* tree, Node* parent_node, int type, tree_type new_val)
{
    assert(tree && parent_node);

    INSERT(right);

    return new_node;
}

Node* tree_node_replace(Tree* tree, Node* old_node, Node* new_node)
{
    assert(tree);

    switch (node_define(old_node))
    {
        case L:
            LEFT(PARENT(old_node)) = new_node;
            break;

        case R:
            RIGHT(PARENT(old_node)) = new_node;
            break;

        default:;
    }

    if (old_node == ROOT(tree))
    {
        tree_set_root(tree, new_node);
        return new_node;
    }

    PARENT(new_node) = PARENT(old_node);

    PARENT(old_node) = NULL;
    tree_erase(tree, old_node);

    return new_node;
}

int tree_erase(Tree* tree, Node* node)
{
    assert(tree);

    int del_vertex_num = 0;
    node_cut(node, &del_vertex_num);

    tree->ver_num += del_vertex_num;

    tree_make_hash(tree);

    return 0;
}

Tree* tree_clear(Tree* tree)
{
    assert(tree);

    int del_vertex_num = 0;
    node_cut(ROOT(tree), &del_vertex_num);

    ROOT(tree) = NULL;

    tree->ver_num = 0;

    tree_make_hash(tree);

    return tree;
}

int node_cut(Node* node, int* out_vertex_num)
{
    assert(out_vertex_num);

    if (!node)
    {
        return 0;
    }

    node_cut(LEFT(node), out_vertex_num);
    node_cut(RIGHT(node), out_vertex_num);

    switch (node_define(node))
    {
        case L:
            LEFT(PARENT(node)) = NULL;
            break;

        case R:
            RIGHT(PARENT(node)) = NULL;
            break;

        default:;
    }

    node_Dtor(node);
    free(node);

    --*out_vertex_num;
    return 0;
}

int node_define(Node* node)                       // get side of the node related to the parent
{
    assert(node);

    if (PARENT(node))
    {
        if (LEFT(PARENT(node)) == node)
            return L;
        if (RIGHT(PARENT(node)) == node)
            return R;
    }
    return NOPARENT;
}

Node* node_find(Node* start_node, tree_type val, int (*info_cmp)(tree_type, tree_type))
{
    assert(info_cmp);

    if (!start_node)
    {
        return NULL;
    }

    if (!info_cmp(val, start_node->info))
    {
        return start_node;
    }
    else
    {
        Node* left = node_find(LEFT(start_node), val, info_cmp);
        if (left)
            return left;

        Node* right = node_find(RIGHT(start_node), val, info_cmp);
        if (right)
            return right;

        return NULL;
    }
}

Node* tree_find_common(Node* node1, Node* node2, int* out_node1_side)              // out_node1_position - side of node1 related to common
{
    assert(node1 && node2);

    size_t height1 = node_get_height(node1);
    size_t height2 = node_get_height(node2);

    while (height1 != height2)
    {
        if (height1 > height2)
        {
            node1 = PARENT(node1);
            height1--;
        }
        else
        {
            node2 = PARENT(node2);
            height2--;
        }
    }

    while (node1 != node2)
    {
        *out_node1_side = node_define(node1);
        node1 = PARENT(node1);
        node2 = PARENT(node2);
    }

    return node1;
}

size_t node_get_height(Node* node)
{
    assert(node);

    size_t height = 0;

    while (PARENT(node))
    {
        node = PARENT(node);
        height++;
    }

    return height;
}

void tree_save(Tree* tree, const char WRITENAME[])
{
    FILE* print_file = fopen(WRITENAME, "w");

    assert(tree && print_file);

    node_save(tree, ROOT(tree), print_file);

    fclose(print_file);
}

#define CASEPRINT(type,state,arg)               case type:\
                                                     fprintf(print_file, state, arg);\
                                                     break;

void node_save(Tree* tree, Node* node, FILE* print_file)
{
    assert(print_file);

    if (!node)
    {
        return;
    }

    fprintf(print_file, "(");

    node_save(tree, LEFT(node), print_file);

    switch(node->type)
    {
        CASEPRINT(OP, "%c", (int) node->info);
        CASEPRINT(CONST_T, "%lg", node->info);
        CASEPRINT(VAR, "%s", tree->variables[(int) node->info]);

        default:
            return;
    }
    node_save(tree, RIGHT(node), print_file);

    fprintf(print_file, ")");
}

Tree* tree_read(const char READNAME[])
{
    Tree* tree = (Tree*) calloc(1, sizeof(*tree));
    tree_Ctor(tree);

    FILE* readtxt = fopen(READNAME, "r");
    assert(readtxt);

    int error = 0;

    tree_set_root(tree, node_read(readtxt, tree, &error));

    if (error)
    {
        fseek(readtxt, error, SEEK_SET);
        printf("~In reading from file \"%s\": char %i(\'%c\') \n\n", READNAME, error, fgetc(readtxt));

        tree_Dtor(tree);
        free(tree);
        fclose(readtxt);
        return NULL;
    }

    fclose(readtxt);
    return tree;
}

#define SKIPSPACES              while(isspace(c = fgetc(read_file)));\
                                ungetc(c, read_file);

#define SYNERROR                *error = ftell(read_file);\
                                tree_erase(tree, new_node);\
                                return NULL;

#define C_READ                  (c = fgetc(read_file))

#define CALL_ERROR              if (*error)\
                                {\
                                    return NULL;\
                                }\

Node* node_read(FILE* read_file, Tree* tree, int* error)
{
    assert(read_file && tree && error);

    CALL_ERROR;

    int c = 0;

    SKIPSPACES;
    if (C_READ == '(')
    {
        Node* new_left = node_read(read_file, tree, error);                     // read the left branch
        CALL_ERROR;

        Node* new_node = get_node(read_file, tree);                             // read current node
        if (!new_node)                                                          // exit if nothing read
            return NULL;

        Node* new_right = node_read(read_file, tree, error);                    // read the right branch
        if(*error)
        {
            tree_erase(tree, new_node);
            return NULL;
        }

        SET_CHILDREN(new_node, new_left, new_right);
        SET_PARENT(new_left, new_node);
        SET_PARENT(new_right, new_node);

        (tree->ver_num)++;

        SKIPSPACES;
        if (C_READ != ')')
        {
            SYNERROR;
        }

        SKIPSPACES;

        return new_node;
    }
    else
    {
        ungetc(c, read_file);
        return NULL;
    }
}

Node* get_node(FILE* read_file, Tree* AST)
{
    assert(read_file && AST);

    int c = 0;
    tree_type info = 0;
    int type = NO_TYPE;

    SKIPSPACES;

    if(isdigit(C_READ))
    {
        ungetc(c, read_file);

        fscanf(read_file, "%lf", &info);
        type = CONST_T;

        return node_create(type, info);
    }


    ungetc(c, read_file);
    type = VAR;
    char var[MAX_VAR_LEN] = "";
    long pos = ftell(read_file);
    fscanf(read_file, "%[^()]s", var);

    #define OP_DEF(operator, priority, read_body, count_body, diff_body, print_body, simplify_body)\
                if(!strnicmp(var, read_body, strlen(var)))\
                {\
                        info = operator;\
                        type = OP;\
                        return node_create(type, info);\
                }

    #include "Operators.h"

    #undef OP_DEF

    fseek(read_file, pos, SEEK_SET);

    fscanf(read_file, "%[A-Za-z_$]s", var);

    int var_index = find_elem(AST->variables, AST->var_count, var);
    if (var_index >= 0)
    {
        info = var_index;
    }
    else
    {
        info = AST->var_count;
        char* new_var = (char*) calloc(strlen(var) + 1, sizeof(*new_var));
        AST->variables[AST->var_count++] = strcpy(new_var, var);                        // put new variable to array of names

        if (AST->var_count >= START_VAR_NUM)
        return NULL;
    }

    return node_create(type, info);
}

//***********************************
/// Finds string element in an array of strings
///
/// \param [in] char** arr - array to find element in
/// \param [in] unsigned int N_arr - number of elements in the array
/// \param [in] char* str - element to find
///
///
/// \return index of the element or -1, if not found
///
//************************************

int find_elem(char** arr, int N_arr, const char* str)
{
    assert(arr && str);

    for (int i = 0; i < N_arr; i++)
    {
        if (arr[i] && (strcmp(arr[i], str) == 0))
        {
            return i;
        }
    }
    return -1;
}


int branch_compare(Node* node1, Node* node2)
{
    if (!node1 && !node2)
    {
        return 1;
    }
    if (node1 && node2 && (node1->type == node2->type) && iszero(node1->info - node2->info))
    {
        // compares the same sub-branches and crossed sub-branches
        if (branch_compare(LEFT(node1), LEFT(node2)) && branch_compare(RIGHT(node1), RIGHT(node2)) || branch_compare(LEFT(node1), RIGHT(node2)) && branch_compare(RIGHT(node1), LEFT(node2)))
        {
            return 1;
        }
    }
    else
    {
        return 0;
    }
}

#define DO_REAL_HASH  \
                                int StructHash_buf = tree->TreeHash_struct;\
                                tree->TreeHash_struct = HASHDEFAULT;\
                                int RealHash_buf = hash (tree, sizeof (*tree));

#define RE_STORE                tree->TreeHash_struct = StructHash_buf;\

#define IF_RETURN(cond)         if (cond) return (cond);

int tree_is_OK(Tree* tree)
{
    assert(tree);
    int error = TRERROK;

    unsigned int counter = 0;

    error = node_is_OK(ROOT(tree), &counter);
    IF_RETURN(error);

    if (tree->ver_num != counter)
    {
        return TRERLEN;
    }

    if (tree->var_count >= START_VAR_NUM)
    {
        return TRERVARLEN;
    }

    DO_REAL_HASH;
    if ((tree->tree_guard_begin != GUARD) || (tree->tree_guard_end != GUARD) ||  RealHash_buf != StructHash_buf)
    {
        return TRERSTRUCT;
    }
    RE_STORE;

    return error;
}

int node_is_OK(Node* node, unsigned int* counter)
{
    if (!node)
    {
        return TRERROK;
    }

    int left_error = node_is_OK(LEFT(node), counter);
    int right_error = node_is_OK(RIGHT(node), counter);

    IF_RETURN(left_error);
    IF_RETURN(right_error);

    if (LEFT(node) && PARENT(LEFT(node)) != node)
        return TRERPOS;

    if (RIGHT(node) && PARENT(RIGHT(node)) != node)
        return TRERPOS;

    if ((node->node_guard_begin != GUARD) || (node->node_guard_end != GUARD))
    {
        return TRERDATA;
    }

    (*counter)++;
    return TRERROK;
}

#define CASE_TREE_OK(ERROR,MESSAGE)\
                                                case ERROR:\
                                                printf("   " MESSAGE "\n");\
                                                break;

int tree_dump(const char dot[], const char DUMPNAME[], Tree* tree)
{
    assert(tree);

    printf("\n~In File: %s\n~In Line: %d\n", __FILE__, __LINE__);
    printf("~AST [0x%X]\n~{\n   Vertex number = %u\n   Root = [0x%X]\n", (out_ptr) tree, tree->ver_num, (out_ptr) ROOT(tree));
    printf("\n   Struct_guard_begin  = %s\n", ((tree->tree_guard_begin) == GUARD) ? "GUARD": "ERROR");
    printf("   Struct_guard_end    = %s\n",   ((tree->tree_guard_end) == GUARD) ? "GUARD": "ERROR");
    printf("\n   Variables = [0x%X]:\n      Number of variables = %u\n", (out_ptr) tree->variables, tree->var_count);

    for (int i = 0; i < tree->var_count; i++)
    {
        printf("      %i: %s\n", i, tree->variables[i]);
    }

    DO_REAL_HASH;
    printf("\n   In memory Struct_Hash = %i\n", StructHash_buf);
    printf("        Real Struct_Hash = %i\n", RealHash_buf);
    RE_STORE;
    printf("\n   INFO: \n");\
    switch(tree_is_OK(tree))
    {
        CASE_TREE_OK(TRERROK,    "Tree is OK");
        CASE_TREE_OK(TRERDATA,   "Data memory in the tree was damaged");
        CASE_TREE_OK(TRERLEN,    "Tree length is not suitable");
        CASE_TREE_OK(TRERPOS,    "Links between tree elements are damaged");
        CASE_TREE_OK(TRERSTRUCT, "Structure of the tree is damaged");
        CASE_TREE_OK(TRERVARLEN, "Too many variables")
    default:
        printf("   Tree verification failed\n");
        return 1;
    }

    printf("~}\n\n");

    if (tree_draw(dot, DUMPNAME, tree))
        return 1;

    return 0;
}

#define NONE_ND(pos)                            strcat(dump_str, #pos "%i[shape = none, label = \"\"];\n %i:<" #pos "> -> " #pos "%i[style = \"invis\"];\n")

#define GET_EDGE(pos)                           strcat(dump_str, "%i:<" #pos "> -> %i ;\n")

#define PRINT_ARG(out_info)                     dump_file, dump_str, node, PARENT(node), node->type, out_info, node, LEFT(node), RIGHT(node)

#define MAKE_VERTEX_STR(color,out_info)         strcat(dump_str, "%i [shape = record, style = filled, fillcolor = \"" #color "\", label = \"{%p | { %i | %" #out_info " }| %p | {<l> %p | <r> %p }} \"];\n")

#define CASE_TYPE(val,color,out_info,body)      case (val):\
                                                        MAKE_VERTEX_STR(color, out_info);\
                                                        body;\
                                                        break

int node_dump(FILE* dump_file, Tree* AST, Node* node)
{
    if (!node)
    {
        return 0;
    }

    char dump_str[MAX_VAR_LEN + 100] = "";

    switch(node->type)
    {
        case OP:
        {
            MAKE_VERTEX_STR(yellow, s);
            const char* out_oper = NULL;

            switch((int) node->info)
            {
                #define OP_DEF(operator, priority, read_body, count_body, diff_body, print_body, simplify_body)\
                case (operator):\
                {\
                    out_oper = read_body;\
                    break;\
                }

                #include "Operators.h"

                #undef OP_DEF
            }

            if(!LEFT(node))
            {
                NONE_ND(l);
                GET_EDGE(r);
                fprintf(PRINT_ARG(out_oper), node, node, node, node, RIGHT(node));
            }
            else
            {
                GET_EDGE(l);
                GET_EDGE(r);
                fprintf(PRINT_ARG(out_oper), node, LEFT(node), node, RIGHT(node));
            }

            break;
        }

        CASE_TYPE(CONST_T, cyan, lg,  fprintf(PRINT_ARG(node->info)));
        CASE_TYPE(VAR, springgreen, s, fprintf(PRINT_ARG(AST->variables[(int) node->info])));

        default:
            return 0;

    }

    node_dump(dump_file, AST, LEFT(node));
    node_dump(dump_file, AST, RIGHT(node));

    return 0;
}

#define TO_GRAPHVIZ(gv_name, DUMPNAME)              strdup(DUMPNAME); strcat(gv_name, ".gv")

#define TO_PNG(png_name, DUMPNAME)                  strdup(DUMPNAME); strcat(png_name, ".png")

#define CONCATINATION(str,arg)                      strcat(str, arg)


int tree_draw(const char dot[], const char DUMPNAME[], Tree* tree)
{
    char* gv_name = TO_GRAPHVIZ(gv_name, DUMPNAME);  // make grathviz file name
    char* png_name = TO_PNG(png_name, DUMPNAME);     // make bmp file name


    FILE* dump_file = fopen(gv_name, "w");
    if (!dump_file) return 1;

    fprintf(dump_file, "digraph ge\n{\n splines = \"polyline\";\n");

    node_dump(dump_file, tree, ROOT(tree));

    fprintf(dump_file, "}");
    fclose(dump_file);


    char* dotty = (char*) calloc(200, sizeof(*dotty));
    dotty = strcpy(dotty, dot);                                     // make
    CONCATINATION(dotty, gv_name);                                  // dot
    //CONCATINATION(dotty, " -o ");                                 // compile
    //CONCATINATION(dotty, png_name);                               // command

    system(dotty);                                      //compile graphviz
    //system(png_name);                                 //open png

    free(gv_name);
    free(png_name);
    free(dotty);

    return 0;
}
