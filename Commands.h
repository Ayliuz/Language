// COMMANDS. H

// COM_DEF(NAME OF FUNCTION, NUMBER OF FUNCTION, TYPE OF ARGUMENTS)
// TYPES OF ARGUMENTS: 0 - NO ARGUMENTS
//                     1 - ARGUMENT TO WORK WITH STACK(register variable, cpu_type value, array index)
//                     3 - ARGUMENTS AS IN 1 WITHOUT cpu_type value
//                     100 - END OF PROGRAM
//                     MARK - STRING ARGUMENT FROM ARRAY OF MARKS

#define REG(k) cpu->r[k]

#define PUSH(arg)   {\
                    cpu_type tempor = arg;\
                    if (tempor > sqrt(DBL_MAX))\
                    {\
                        printf("~Invalid value in %i line\n", com_num); \
                        return 1;\
                    }\
                    stack_push(cpu->data, tempor);\
                    }

#define POP stack_pop(cpu->data)

#define CASEPUSH(WhatType,argument)   case (WhatType): {PUSH(argument); break;}

#define CASEPOP(WhatType,argument)    case (WhatType): {argument = stack_pop(cpu->data); break;}

#define CASEIN(WhatType,argument)     case (WhatType): {printf("? "); scanf("%lf", argument); break;}

#define DO_MARK  {ip = comFile + (*((long long*) argum) - 1) * COM_SIZE;}

#define DEFAULT  default:\
                    printf("~Invalid syntax in %i line\n", com_num);\
                    return 1;

#define INITPOP cpu_type a = POP; cpu_type b = POP; PUSH(b); PUSH(a);

#define CHECK_RAM_SIZE(typed, arg) (type == typed && arg >= RAM_SIZE)

#define INDEX   size_t index = (unsigned int) ((int) *((cpu_type*) &argum));\
                if (CHECK_RAM_SIZE(ARR_ARG, index) || CHECK_RAM_SIZE(ARR_REG, cpu->r[registr]))\
                {\
                    printf("~Invalid index of RAM in %i line\n", com_num);\
                    return 1;\
                }

COM_DEF(push, 1, 1,
            switch (type)
            {
                case (ARR_ARG): {INDEX; PUSH(cpu->Ram[index]); break;}

                CASEPUSH(ARGUM, *((cpu_type*) &argum));

                CASEPUSH(REGIST, REG(registr));

                CASEPUSH(ARR_REG, cpu->Ram[(int) REG(registr)]);

                DEFAULT
            }
        )

COM_DEF(pop, 2, 3,
            switch (type)
            {
                case (NON_ARG): POP; break;

                case (ARGUM): POP; break;

                CASEPOP(REGIST, REG(registr));

                case (ARR_ARG): {INDEX; cpu->Ram[index] = stack_pop(cpu->data); break;}

                CASEPOP(ARR_REG, cpu->Ram[(int) REG(registr)]);

                DEFAULT
            }
        )

COM_DEF(add, 3, 0, PUSH(POP + POP);)

COM_DEF(mul, 4, 0, PUSH(POP * POP);)

COM_DEF(jump, 5, MARK, DO_MARK)

COM_DEF(ifg, 6, MARK,  INITPOP; if(a < b) DO_MARK;)

COM_DEF(ifl, 7, MARK,  INITPOP; if(a > b) DO_MARK;)

COM_DEF(ifeq, 8, MARK, INITPOP; if(iszero(a - b))   DO_MARK;)

COM_DEF(ifneq, 9, MARK,INITPOP; if(!iszero(a - b))  DO_MARK;)

COM_DEF(call, 10, MARK, stack_push(cpu->func_calls, com_num); DO_MARK;)

COM_DEF(ret, 11, 0,
            if (cpu->func_calls->StackSize > 0)
            {
                *((long long*)(argum)) = stack_pop(cpu->func_calls) + 1;
                DO_MARK;
            }
            else
            {
                free(comFile); return 0;
            }
        )

COM_DEF(out, 12, 0,  if (cpu->data->StackSize > 0) printf("%.7lg\n", POP);)

COM_DEF(in, 13, 3,
            switch (type)
            {
                case(NON_ARG):
                {
                    cpu_type buf = 0;
                    printf("? ");
                    scanf("%lf", &buf);
                    PUSH(buf);
                    break;
                }
                CASEIN(REGIST,    &(REG(registr)));

                case (ARR_ARG): {INDEX; printf("? "); scanf("%lf", &(cpu->Ram[index])); break;}

                CASEIN(ARR_REG,   &(cpu->Ram[(int) REG(registr)]));

                DEFAULT
            }
        )

COM_DEF(end, 100, 0, free(comFile); return 0;)

COM_DEF(subtr, 14, 0, INITPOP; POP; POP; PUSH(b - a);)

COM_DEF(div, 15, 0,
            INITPOP;
            POP;
            POP;
            if (!iszero(a))
            {
                PUSH(b/a);
            }
            else
            {
                printf("~Division by zero in %i line\n", com_num);
                return 1;
            }
        )

COM_DEF(sqrt, 16, 0, PUSH(sqrt(POP));)

COM_DEF(pow,  17, 0, INITPOP; POP; POP; PUSH(pow(b, a));)

COM_DEF(meow, 18, 0, printf("               )\\._.,--....,'``.       \n .b--.        /;   _.. \\   _\\  (`._ ,. \n`=,-,-'~~~   `----(,_..'--(,_..'`-.;.'\n");)

#undef CHECK_RAM_SIZE
#undef INDEX
#undef INITPOP
#undef DEFAULT
#undef DO_MARK
#undef CASEIN
#undef CASEPOP
#undef CASEPUSH
#undef POP
#undef PUSH
#undef REG
