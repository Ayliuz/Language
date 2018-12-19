// OPERATORS.H


// OP_DEF(operator, priority, read_body, count_body, diff_body, print_body, simplify_body)

#define OPERATE(operator, node_left, node_right)            operate_branch_create(&ver_num_increase, operator, node_left, node_right)

#define DIF(node)                   leaf_diff(var_index, node, out_vertex_num)

#define COPY(node)                  branch_copy(node, &ver_num_increase)

#define PRINT_LEFT                  leaf_print(print_file, AST, LEFT(node))

#define PRINT_RIGHT                 leaf_print(print_file, AST, RIGHT(node))

#define IS_ZERO(node)               ((node->type == CONST_T) && iszero(node->info))

#define IS_ONE(node)                ((node->type == CONST_T) && iszero(node->info - 1))

#define IS_ZERO_BODY(node_zero, new_branch) \
                                    {\
                                        node_cut(node_zero, out_vertex_num);\
                                        \
                                        Node* new_branch_templ = new_branch;\
                                        \
                                        node->type = new_branch_templ->type;\
                                        node->info = new_branch_templ->info;\
                                        LEFT(node) = LEFT(new_branch_templ);\
                                        RIGHT(node)= RIGHT(new_branch_templ);\
                                        \
                                        if (LEFT(new_branch_templ))\
                                        {\
                                            PARENT(LEFT(new_branch_templ)) = node;\
                                        }\
                                        if (RIGHT(new_branch_templ))\
                                        {\
                                            PARENT(RIGHT(new_branch_templ)) = node;\
                                        }\
                                        \
                                        node_Dtor(new_branch_templ);\
                                        free(new_branch_templ);\
                                        --*out_vertex_num;\
                                        break;\
                                    }

#define COUNT_NODE(expr)            {\
                                        node->info = expr;\
                                        node->type = CONST_T;\
                                        node_cut(LEFT(node), out_vertex_num);\
                                        node_cut(RIGHT(node), out_vertex_num);\
                                        break;\
                                    }

#define COUNT_CHILDREN(expr)        if (LEFT(node)->type == CONST_T && RIGHT(node)->type == CONST_T)\
                                    {\
                                        COUNT_NODE(expr);\
                                    }

#define EQUAL_CHILDREN              branch_compare(RIGHT(node), LEFT(node))

OP_DEF(SUM, 3, "+", result = LEFT_COUNT + RIGHT_COUNT,

            NEW_NODE(OPERATE(SUM, DIF(LEFT(node)), DIF(RIGHT(node))));,

            PRINT_LEFT;
            FPRINTF("+");
            PRINT_RIGHT;,

            {
                COUNT_CHILDREN(LEFT(node)->info + RIGHT(node)->info);

                if (EQUAL_CHILDREN)
                {
                    node->info = MUL;
                    node_cut(LEFT(node), out_vertex_num);
                    LEFT(node) = node_create(CONST_T, 2);
                    ++*out_vertex_num;
                    SET_PARENT(LEFT(node), node);
                    break;
                }

                if (IS_ZERO(LEFT(node)))
                {
                    IS_ZERO_BODY(LEFT(node), RIGHT(node));
                }

                if (IS_ZERO(RIGHT(node)))
                {
                    IS_ZERO_BODY(RIGHT(node), LEFT(node));
                }

                if (LEFT(node)->type == CONST_T && RIGHT(node)->type == VAR)
                {
                    node_swap(RIGHT(node), LEFT(node));
                }
            }
        )


OP_DEF(SUB, 3, "-", result = LEFT_COUNT - RIGHT_COUNT,

            NEW_NODE(OPERATE(SUB, DIF(LEFT(node)), DIF(RIGHT(node))));,

            PRINT_LEFT;
            FPRINTF("-");
            PRINT_RIGHT;,

            {
                COUNT_CHILDREN(LEFT(node)->info - RIGHT(node)->info);

                if (EQUAL_CHILDREN)
                {
                    COUNT_NODE(0);
                }

                if (IS_ZERO(LEFT(node)))
                {
                    node->info = MUL;
                    LEFT(node)->info = -1;
                    break;
                }

                if (IS_ZERO(RIGHT(node)))
                {
                    IS_ZERO_BODY(RIGHT(node), LEFT(node));
                }
            }
        )


OP_DEF(MUL, 2, "*", result = LEFT_COUNT * RIGHT_COUNT,

            Node* left_part_branch = OPERATE(MUL, DIF(LEFT(node)), COPY(RIGHT(node)));
            Node* right_part_branch = OPERATE(MUL, COPY(LEFT(node)), DIF(RIGHT(node)));
            NEW_NODE(OPERATE(SUM, left_part_branch, right_part_branch));,

            PRINT_LEFT;
            if (LEFT(node)->type != CONST_T || RIGHT(node)->type != VAR)
            {
                FPRINTF(" \\cdot ");
            }
            PRINT_RIGHT;,

            {
                COUNT_CHILDREN(LEFT(node)->info * RIGHT(node)->info);

                if (EQUAL_CHILDREN)
                {
                    node->info = POW;
                    node_cut(RIGHT(node), out_vertex_num);
                    RIGHT(node) = node_create(CONST_T, 2);
                    ++*out_vertex_num;
                    SET_PARENT(RIGHT(node), node);
                    break;
                }

                if (IS_ZERO(LEFT(node)))
                {
                    COUNT_NODE(0);
                }

                if (IS_ZERO(RIGHT(node)))
                {
                    COUNT_NODE(0);
                }

                if (IS_ONE(LEFT(node)))
                {
                    IS_ZERO_BODY(LEFT(node), RIGHT(node));
                }

                if (IS_ONE(RIGHT(node)))
                {
                    IS_ZERO_BODY(RIGHT(node), LEFT(node));
                }

                if (RIGHT(node)->type == CONST_T && LEFT(node)->type == VAR)
                {
                    node_swap(RIGHT(node), LEFT(node));
                }
            }
        )


OP_DEF(DIV, 2, "/", result = LEFT_COUNT / RIGHT_COUNT,

            Node* left_top_branch = OPERATE(MUL, DIF(LEFT(node)), COPY(RIGHT(node)));
            Node* right_top_branch = OPERATE(MUL, COPY(LEFT(node)), DIF(RIGHT(node)));
            Node* bottom_branch = OPERATE(MUL, COPY(RIGHT(node)), COPY(RIGHT(node)));
            Node* fract_branch = OPERATE(DIV, OPERATE(SUB, left_top_branch, right_top_branch), bottom_branch);
            NEW_NODE(fract_branch);,

            FPRINTF(" \\frac{");
            PRINT_LEFT;
            FPRINTF("}{");
            PRINT_RIGHT;
            FPRINTF("}");,

            {
                if(!iszero(RIGHT(node)->info))
                {
                    COUNT_CHILDREN(LEFT(node)->info / RIGHT(node)->info);
                    if (EQUAL_CHILDREN)
                    {
                        COUNT_NODE(1);
                    }
                }

                if (IS_ZERO(LEFT(node)))
                {
                    COUNT_NODE(0);
                }

                if (IS_ONE(RIGHT(node)))
                {
                    IS_ZERO_BODY(RIGHT(node), LEFT(node));
                }

            }
        )

OP_DEF(SIN, 1, "sin", sin(RIGHT_COUNT),                                             // right branch is argument

            Node* sin_diff = OPERATE(COS, NULL, COPY(RIGHT(node)));
            Node* arg_diff = DIF(RIGHT(node));
            NEW_NODE(OPERATE(MUL, arg_diff, sin_diff));,

            FPRINTF(" \\sin ");
            if (RIGHT(node)->type != CONST_T && RIGHT(node)->type != VAR)
            {
                FPRINTF(" \\left( ");
                PRINT_RIGHT;
                FPRINTF(" \\right) ");
            }
            else
            {
                PRINT_RIGHT;
            },

            {
                if (RIGHT(node)->type == CONST_T)
                    COUNT_NODE(sin(RIGHT(node)->info));
            }

        )

OP_DEF(COS, 1, "cos", cos(RIGHT_COUNT),                                         // right branch is argument

            Node* cos_diff_abs = OPERATE(SIN, NULL, COPY(RIGHT(node)));
            Node* cos_diff = OPERATE(MUL, node_create(CONST_T, -1), cos_diff_abs);
            ++*out_vertex_num;
            Node* arg_diff = DIF(RIGHT(node));
            NEW_NODE(OPERATE(MUL, arg_diff, cos_diff));,

            FPRINTF(" \\cos ");
            if (RIGHT(node)->type != CONST_T && RIGHT(node)->type != VAR)
            {
                FPRINTF(" \\left( ");
                PRINT_RIGHT;
                FPRINTF(" \\right) ");
            }
            else
            {
                PRINT_RIGHT;
            },

            {
                if (RIGHT(node)->type == CONST_T)
                    COUNT_NODE(cos(RIGHT(node)->info));
            }

        )

OP_DEF(TAN, 1, "tan", tan(RIGHT_COUNT),                                         // right branch is argument


            Node* cos_arg = OPERATE(COS, NULL, COPY(RIGHT(node)));
            Node* cos_arg_sqr = OPERATE(MUL, cos_arg, COPY(cos_arg));
            Node* arg_diff = DIF(RIGHT(node));
            NEW_NODE(OPERATE(DIV, arg_diff, cos_arg_sqr));,

            FPRINTF(" \\tan ");
            PRINT_RIGHT;,

            {
                if (RIGHT(node)->type == CONST_T)
                    COUNT_NODE(tan(RIGHT(node)->info));
            }

        )

OP_DEF(LN, 1, "ln", log(RIGHT_COUNT),

            NEW_NODE(OPERATE(DIV, DIF(RIGHT(node)), COPY(RIGHT(node)))),

            FPRINTF(" \\ln ");
            PRINT_RIGHT;,

            {
                if (RIGHT(node)->type == CONST_T)
                    COUNT_NODE(log(RIGHT(node)->info));

                if (RIGHT(node)->type == OP && OPER_VAL(RIGHT(node))== EXP)
                {
                    Node* new_branch = RIGHT(RIGHT(node));
                    RIGHT(RIGHT(node)) = NULL;
                    IS_ZERO_BODY(RIGHT(node), new_branch);
                }
            }
        )

OP_DEF(EXP, 1, "exp", exp(RIGHT_COUNT),

            NEW_NODE(OPERATE(MUL, DIF(RIGHT(node)), COPY(node)));,

            if (RIGHT(node)->type != CONST_T && RIGHT(node)->type != VAR)
            {
                FPRINTF(" \\exp{ ");
                PRINT_RIGHT;
                FPRINTF(" } ");
            }
            else
            {
                FPRINTF(" e^{ ");
                PRINT_RIGHT;
                FPRINTF(" } ");
            },

            {
                if (RIGHT(node)->type == CONST_T)
                    COUNT_NODE(exp(RIGHT(node)->info));

                if (RIGHT(node)->type == OP && OPER_VAL(RIGHT(node))== LN)
                {
                    Node* new_branch = RIGHT(RIGHT(node));
                    RIGHT(RIGHT(node)) = NULL;
                    IS_ZERO_BODY(RIGHT(node), new_branch);
                }
            }
        )


#define TRIGON_POW_PRINT    PRINT_RIGHT;\
                            FPRINTF("} ");\
                            leaf_print(print_file, AST, RIGHT(LEFT(node)));\

OP_DEF(POW, 1, "^", pow(LEFT_COUNT, RIGHT_COUNT),

            Node* same =            COPY(node);
            Node* left_part_diff =  OPERATE(MUL, DIF(RIGHT(node)), OPERATE(LN, NULL, COPY(LEFT(node))));
            Node* right_part_diff = OPERATE(MUL, DIF(LEFT(node)), OPERATE(DIV, COPY(RIGHT(node)), COPY(LEFT(node))));
            Node* sum_diff = OPERATE(SUM, left_part_diff, right_part_diff);
            NEW_NODE(OPERATE(MUL, same, sum_diff));,

            if (LEFT(node)->type != CONST_T && LEFT(node)->type != VAR)
            {
                if (RIGHT(node)->type == CONST_T && LEFT(node)->type == OP)
                {
                    switch(OPER_VAL(LEFT(node)))
                    {
                        case SIN:
                            FPRINTF(" \\sin^{");
                            TRIGON_POW_PRINT;
                            break;

                        case COS:
                            FPRINTF(" \\cos^{");
                            TRIGON_POW_PRINT;
                            break;

                        case TAN:
                            FPRINTF(" \\tan^{");
                            TRIGON_POW_PRINT;
                            break;

                        default:
                            PRINT_LEFT;
                            FPRINTF("^{");
                            PRINT_RIGHT;
                            FPRINTF("} ");
                    }
                }
                else
                {
                    PRINT_LEFT;
                    FPRINTF("^{");
                    PRINT_RIGHT;
                    FPRINTF("} ");
                }
            }
            else
            {
                PRINT_LEFT;
                FPRINTF("^{");
                PRINT_RIGHT;
                FPRINTF("} ");
            },

            {
                COUNT_CHILDREN(pow(LEFT(node)->info, RIGHT(node)->info));

                if (IS_ZERO(LEFT(node)))
                {
                    COUNT_NODE(0);
                }

                if (IS_ZERO(RIGHT(node)))
                {
                    COUNT_NODE(1);
                }

                if (IS_ONE(LEFT(node)))
                {
                    COUNT_NODE(1);
                }

                if (IS_ONE(RIGHT(node)))
                {
                    IS_ZERO_BODY(RIGHT(node), LEFT(node));
                }
            }

        )
