// ASSEMBLER.CPP

#include "assembler.h"

//***********************************
/// Reads assembler-program code from the file and translates it into cpu code
///
/// \param [in] const char READNAME[] - name of the file to read from
/// \param [in] const char COMNAME[] - name of the file to write
///
/// \return 0 if translation was correct, 1 - if not
///
//************************************

int assembler(const char READNAME[], const char COMNAME[])
{
    MyText* Program = read_in_Text(READNAME, READ);
    if (!Program) {                                                                      printf("~Error in reading from \"%s\" file\n", READNAME); return 1;}

    char* assembl = (char*) calloc (Program->N_table * COM_SIZE + 1, sizeof (*assembl));
    char** marks = (char**) calloc (Program->N_table + 1, sizeof(*marks));

    if (put_commands(Program, assembl, marks)) return 1; // check if commands are correct
    if (put_commands(Program, assembl, marks)) return 1; // fulfill the marks

    if (write_to_file(COMNAME, assembl, Program->N_table * COM_SIZE, DELANDWRITEBYTES)) {printf("~Error in writing to \"%s\" file\n", COMNAME); return 1;}

    free(assembl);
    free_table(marks, Program->N_table + 1, OWNMEM);
    free(marks);
    text_Dtor(Program);
    free(Program);
    Program = NULL;
    return 0;
}

//***********************************
/// Recognize type of command on the string and its arguments
///
/// \param [in] char* str - the string to read from
/// \param [out] char registr - number of register variable used in command
/// \param [out] char* arg - pointer to write argument of command
///
/// \return the type of command from the string
///
//************************************

#define FIND(ch) (strchr(str,(ch)) != NULL)
#define PASS_COMMENT if (FIND(';'))\
                        {\
                            *(strchr(str, ';')) = '\0';\
                        }
#define PASS_SPACES while (isspace(*(++str)))
#define DIG(num) if (isdigit(*str) || (*str) == '-')\
                    {\
                        type = num;\
                        sscanf(str, "%lf", (cpu_type*) arg);\
                    }
#define ALPH(num) if ((*str == 'r'))\
                    {\
                        type = num;\
                        sscanf(str + 1, "%[0-9]s", registr);\
                    }

int funct_type (char* str, char* registr, char* arg)
{
    assert(str && registr && arg);

    int type = 0;

    str = skip_cmd_spaces_and_comment(str);
    if (*str == '\0') return NON_ARG;

    if ((*str == '[') && FIND(']'))
    {
        PASS_SPACES;
        ALPH(ARR_REG)
        else DIG(ARR_ARG)
        else type = ERRORTYPE;
    }
    else
    {
        ALPH(REGIST)
        else DIG(ARGUM)
        else type = ERRORTYPE;
    }
    return type;
}

//***********************************
/// Recognize if the string is mark or not
///
/// \param [in] char* str - the string to recognize
/// \param [out] char* *mark - pointer to write the name of mark for this command
///
/// \return 1 if the string is a mark, 0 if not
///
//************************************

int ismark (char* str, char* *mark)
{
    assert(str && mark);

    PASS_COMMENT;
    str--;
    PASS_SPACES;
    if ((*str == ':'))
    {
        PASS_SPACES;
        if (strlen(str) > sizeof(cpu_type)) str[sizeof(cpu_type)] = '\0';
        *mark = strdup(str);
        return 1;
    }
    return 0;
}

//***********************************
/// Gets mark name from the string
///
/// \param [in] char* str - the string to get name from
/// \param [out] char* arg - string to write the name of mark to
///
/// \return 0 if there is mark, 1 if not
///
//************************************

int get_mark (char* str, char* arg)
{
    assert(str && arg);

    str = skip_cmd_spaces_and_comment(str);
    if (*str == '\0') return MISSMARK;
    sscanf(str, "%8s", arg);
    return 0;
}


//***********************************
/// Skip letters and spaces in the beginning of the string and comments
///
/// \param [in] char* s - the string to read from
///
///
/// \return pointer to first non-letter character or '\0' if there is none of them excluding ';' and the whole comments
///
//************************************

char* skip_cmd_spaces_and_comment(char* str)
{
    assert(str);

    for (; *(str) != '0' ; ++str)
    {
        if(!isalpha(*str)) break;
    }
    str--;
    PASS_SPACES;
    PASS_COMMENT;
    return str;
}

#undef FIND
#undef DIG
#undef ALPH
#undef PASS_COMMENT
#undef PASS_SPACES

//***********************************
/// Compares the beginning of a string with command
///
/// \param [in] char* command - the command to compare with
/// \param [in] char* str - the string to compare
///
///
/// \return 1 if command are suitable, 0 if not
///
//************************************

int command_cmp (const char* command, char* str)
{
    assert(str);

    for (unsigned int i = 0; i < strlen(command); i++)
    {
        if (command[i] != str[i]) return 0;
    }
    return 1;
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

//***********************************
/// Get commands from Text file and puts it translated to cpu code to array
///
/// \param [in] MyText* Program - MyText object to read from
/// \param [out] char* assembl - array to put cpu code
/// \param [in/out] char** marks - array of names of the marks
///
/// \return 1 if there were any errors, 0 if not
///
//************************************

#define COM_DEF(name,num,oper,body) if(command_cmp(#name, Program->text_table[index])) \
                                  {\
                                        com = COM_##name;\
                                        operands = oper;\
                                  }

#define PRINTSTRING printf("--->\"%s\"\n", Program->text_table[index])

int put_commands(MyText* Program, char* assembl, char** marks)
{
    assert(Program && assembl && marks);

    enum AllCommand com = COM_DEFAULT;
    char* regbuf = (char*) calloc(4, sizeof(*regbuf));
    char* argum = (char*) calloc(sizeof(cpu_type) + 1, sizeof(*argum));

    int str_i = 0;
    int index = 0;
    while (index < (Program->N_table))
    {
        if (!(*Program->text_table[index]))
        {
            index++;
            continue;
        }

        char registr = 0;
        char type = 0;
        int operands = -1;

        if (ismark(Program->text_table[index],  &(marks[str_i])))
        {
            index++;
            continue;
        }

        #include "Commands.h"

        switch (operands)
        {
            case 1:
                if ((type = funct_type(Program->text_table[index], regbuf, argum)) == ERRORTYPE || type == NON_ARG) {printf("~Not suitable argument in %i line\n", index); PRINTSTRING; return 1;}
                registr = atoi(regbuf);
                break;

            case MARK:
                type = MARK;
                if (get_mark(Program->text_table[index], argum) == MISSMARK) {                                       printf("~Missed argument in %i line\n", index); PRINTSTRING; return 1;}
                *((long long*)(argum)) = find_elem(marks, Program->N_table, argum);
                break;

            case 0:
                type = NON_ARG;
                break;

            case 3:
                if ((type = funct_type(Program->text_table[index], regbuf, argum)) == ERRORTYPE) {                   printf("~Not suitable argument in %i line\n", index); PRINTSTRING; return 1;}
                registr = atoi(regbuf);
                break;

            default:
                if(strlen(Program->text_table[index]) > 0) {                                                         printf("~Unknown command in %i line:\n", index); PRINTSTRING; return 1;}
        }

        char* asm_point = (assembl + str_i * COM_SIZE); // write info in cpu code array
        *(asm_point) = (char) com;
        *(asm_point + 1) = type;
        *(asm_point + 2) = registr;
        memmove(asm_point + 3, argum, sizeof(cpu_type));

        memset(regbuf, '\0', 4);
        memset(argum, '\0', sizeof(cpu_type) + 1);

        ++index;
        ++str_i;
    }

    free(argum);
    free(regbuf);
    return 0;
}

#undef COM_DEF
#undef PRINTSTRING
