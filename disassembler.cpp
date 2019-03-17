// DISASSEMBLER. CPP

#include "assembler.h"

//***********************************
/// Reads cpu code from the file and translates it into assembler
///
/// \param [in] const char READNAME[] - name of the file to read from
/// \param [in] const char WRITENAME[] - name of the file to write
/// \param [in] const int mode - mode to write: PROG - write only disassemble program, LOG - write as log file (with cpu code)
///
/// \return 0 if translation was correct, 1 - if not
///
//************************************

int disassembler(const char READNAME[], const char WRITENAME[], const int mode)
{
    char* comFile = read_from_file(READNAME, READBYTES);

    int len = txt_file_length(READNAME);
    assert(!(len % COM_SIZE));

    size_t n_str = len /  COM_SIZE;

    MyText Code;
    Code.text_table = (char**) calloc ( n_str + 1, sizeof(*Code.text_table));
    Code.N_table = n_str;

    int* marks = (int*) calloc (n_str + 1, sizeof(*marks));

    if (get_commands(comFile, len, &Code, marks)) return 1;
    if (out_disasm(WRITENAME, &Code, marks, comFile, mode)) return 1;

    free(comFile);
    free(marks);
    free_table(Code.text_table, Code.N_table, OWNMEM);
    return 0;
}

//***********************************
/// Get cpu code from array and puts it translated to MyText object
///
/// \param [in] const char* program - array to read from
/// \param [in] const size_t len - length of the array
/// \param [out] MyText* Code - MyText object to write to
/// \param [out] int* marks - array of occurring marks
///
///
/// \return 1 if there were any errors, 0 if not
///
//************************************

#define COM_DEF(name, num, oper, body) if(*point == (char) (num)) \
                                  {\
                                        free(com_name);\
                                        com_name = strdup(#name);\
                                        operands = (oper);\
                                  }
#define IFPRINTS(WhatType, format, arguments)  if (type == (WhatType)) sprintf(str, format, com_name, arguments)

int get_commands(char* program, const size_t len, MyText* Code, int* marks)
{
    assert(program && Code && marks);

    char* com_name = NULL;
    char* argum = (char*) calloc(sizeof(cpu_type), sizeof(*argum));
    int mark = 0;
    int counter = 0;

    for(char* point = program; point < (program + len); point += COM_SIZE, counter++)
    {
        if (!(*point)) continue;

        int type = *(point + 1);
        int registr = *(point + 2);
        int operands = -1;
        memmove(argum, point + 3, sizeof(cpu_type));

        char* str = (char*) calloc( STRLEN, sizeof(*str));

        #include "Commands.h"

        switch (operands)
        {
            case 3:
                PASS_CASE;

            case 1:
                if (type == NON_ARG) sprintf(str,  "%s", com_name);
                IFPRINTS(ARGUM,     "%s %.4f",     *((cpu_type*) argum));
                IFPRINTS(REGIST,    "%s r%i",       registr);
                IFPRINTS(ARR_ARG,   "%s [%.0lf]",   *((cpu_type*) argum));
                IFPRINTS(ARR_REG,   "%s [r%i]",     registr);
                break;

            case MARK:
            {
                long long buf = *((long long*) argum);
                marks[buf] = (marks[buf] == 0) ? ++mark: marks[buf];
                IFPRINTS(MARK,      "%s :%i",       marks[buf]);
                break;
            }

            case 0:
                sprintf(str,  "%s", com_name);
                break;

            default:
                printf("~Damaged file(%i position)\n", (int) (point - program));
                return 1;
        }

        Code->text_table[counter] = str;
        memset(argum, '\0', sizeof(cpu_type));
    }

    free(com_name);
    free(argum);

    return 0;
}

#undef COM_DEF
#undef IFPRINTS

//***********************************
/// Writes translated assemble program to file
///
/// \param [in] const char* name - name of the file to write
/// \param [in] const MyText* Code - MyText object to write from
/// \param [in] const int* marks - array of occurring marks
/// \param [in] const char* program - array of cpu code for log file
/// \param [in] const int mode - mode to write: PROG - write only disassemble program, LOG - write as log file (with cpu code)
///
/// \return 1 if there were any errors, 0 if not
///
//************************************

#define FPRINT(format, argum) fprintf(txtFile, format, argum);

int out_disasm(const char* name, const MyText* Code, const int* marks, const char* program, const int mode)
{
    assert(Code && marks && name);
    FILE* txtFile = fopen(name, "w");
    if (!txtFile) return 1;

    size_t ind = 0;
    char cpu_code[COM_SIZE] = "";

    while (ind < Code->N_table)
    {
        if (marks[ind] > 0)
        {
            fprintf(txtFile, "\n                                                          :%i\n", marks[ind]);
        }

        if(!Code->text_table[ind])
        {
            ind++;
            continue;
        }

        FPRINT("%-6u", ind)
        if (mode)
        {
            memmove(cpu_code, (program + ind * COM_SIZE), COM_SIZE);
            for (size_t i = 0; i < COM_SIZE; ++i) FPRINT("%3.2X ", (unsigned char) cpu_code[i]);
        }
        FPRINT ("\t | %s\n", Code->text_table[ind]);
        ind++;
    }

    fclose(txtFile);
    return 0;
}

#undef FPRINT
