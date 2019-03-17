// DATA. H

#ifndef DATA_H_INCLUDED
#define DATA_H_INCLUDED

const char COMNAME[]  =     "done.asm";
const char WRITENAME[] =    "log.txt";
const char STACK_DUMP[] =   "dump.txt";
typedef double cpu_type;

const size_t COM_SIZE = sizeof(char) + sizeof(char) + sizeof(char) + sizeof(cpu_type); // ����� ������, ��� (��������), �������, �������� (8 ���)


#define COM_DEF(name,num,oper,body) COM_##name = (num),
///������������ ��� ������ �� �������
enum AllCommand
{
    COM_DEFAULT,
    #include "Commands.h"
    COM_MAX
};
#undef COM_DEF

///����������� ����������
enum Regist
{
    r1 = 1, r2, r3, r4, r5, r6, r7, r8, r9, r10
};

#endif // DATA_H_INCLUDED
