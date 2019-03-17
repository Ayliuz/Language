// CPU. CPP

#include "CPU.h"

//************************************
/// Constructs CPU object and initialize it.
///
/// \param [in] CPU* cpu - pointer to CPU object
///
/// \return void
//************************************


void cpu_Ctor(CPU* cpu)
{
    assert(cpu);

    free(cpu->Ram);
    cpu->Ram = (cpu_type*) calloc(RAM_SIZE, sizeof(*(cpu->Ram)));

    for(int reg = 0; reg < REGISTER_SIZE; reg++)
        cpu->r[reg] = 0;

    cpu->data = (MyStack*) calloc(1, sizeof(*(cpu->data)));
    stack_Ctor(cpu->data);

    cpu->func_calls = (MyStack*) calloc(1, sizeof(*(cpu->func_calls)));
    stack_Ctor(cpu->func_calls);
}

//************************************
/// Deletes CPU object and its data.
///
/// \param [in] CPU* cpu - pointer to CPU object
///
///
/// \return void
///
//************************************


void cpu_Dtor(CPU* cpu)
{
    assert(cpu);

    memset(cpu->Ram, NAN, RAM_SIZE);
    free(cpu->Ram);

    for(int reg = 0; reg < REGISTER_SIZE; reg++)
        cpu->r[reg] = NAN;

    stack_Dtor(cpu->data);
    free(cpu->data);

    stack_Dtor(cpu->func_calls);
    free(cpu->func_calls);

    cpu = NULL;
}

//***********************************
/// Get cpu code from array and perform the program
///
/// \param [in] const char READNAME[] - name of the file to read from
/// \param [in] const char DUMPNAME[] - name of the file for logs
/// \param [in/out]  CPU* cpu - pointer to CPU object
///
/// \return 1 if there were any errors, 0 if not
///
//************************************

#define COM_DEF(name, num, oper, body) { case num:\
                                            body\
                                            break;\
                                       }

int cpu_get_commands(const char READNAME[], const char DUMPNAME[], CPU* cpu)
{
    char* comFile = read_from_file(READNAME, READBYTES);

    int len = txt_file_length(READNAME);
    assert(!(len % COM_SIZE) && cpu);

    char* argum[sizeof(cpu_type)] = {};

    for (char* ip = comFile; ip < (comFile + len); ip += COM_SIZE)
    {
        int com_num = (ip - comFile) / COM_SIZE;            // need
                                                            // in
        int type = *(ip + 1);                               // Commands.h
        int registr = *(ip + 2);                            //
        memmove(&argum, ip + 3, sizeof(cpu_type));

        unsigned int com = (unsigned int) *ip;

        switch(com)
        {
            #include "Commands.h"
        }

        if (type == ARR_ARG || type == ARR_REG)
            Sleep(2);

        dump(DUMPNAME, cpu, com_num);
    }
    free(comFile);
    return 0;
}

#undef COM_DEF

//***********************************
/// Writes logs to file
///
/// \param [in] const char DUMPNAME[] - name of the file for logs
/// \param [in] MyStack* stck - pointer to MyStack object for logging
/// \param [in] int command - number of current command
///
/// \return 1 if there were any errors,0 if not
///
//************************************

int dump(const char DUMPNAME[], CPU* cpu, int command)
{
    assert(cpu);
    FILE* dumptxt = (command == 0) ? fopen(DUMPNAME, "w") : fopen(DUMPNAME, "a");
    if (!dumptxt) return 1;

    fprintf(dumptxt, "In File:  %s\n           In command:  %i\n", __FILE__, command);
    fprintf(dumptxt, "   CPU->data:\n      Capacity = %u\n      Size = %u\n      data [0x%X]:\n", cpu->data->StackCapacity, cpu->data->StackSize, cpu->data->data);

    // stack output
    int bufn = cpu->data->StackSize;
    for (int i = 0; i < bufn; i++)
    {
        fprintf(dumptxt, "         data[%d] = %f\n", i, cpu->data->data[i]);
    }
    fprintf(dumptxt, "\n\n");


    //register output
    fprintf(dumptxt, "   Register:\n");
    for(int ind = 0; ind < REGISTER_SIZE; ind++)
    {
        fprintf(dumptxt, "         r[%i] = %lf\n", ind, cpu->r[ind]);
    }
    fprintf(dumptxt, "\n\n");

    // RAM output
    fprintf(dumptxt, "   RAM:\n");
    for(int point = 0; point < RAM_SIZE; point++)
    {
        if(cpu->Ram[point])
            fprintf(dumptxt, "         RAM[%i] = %lf\n", point, cpu->Ram[point]);
    }
    fprintf(dumptxt, "\n\n");


    fclose(dumptxt);
    return 0;
}
