//TEXT. CPP

#include "text.h"

//************************************
/// Constructs Text structure and initialize it.
///
/// \param [in] struct Text* textobj - pointer to Text structure
///
///
/// \return void
///
//************************************

void text_Ctor(MyText* textobj)
{
    assert(textobj);

    free(textobj->text_table);
    textobj->text_table = (char**) calloc(StartLength, sizeof(*(textobj->text_table)));

    textobj->N_table = StartLength;

}

//************************************
/// Deletes Text structure and its data.
///
/// \param [in] struct Text* textobj - pointer to Text structure
///
///
/// \return void
///
//************************************

void text_Dtor(MyText* textobj)
{
    assert(textobj);

    free_table(textobj->text_table, textobj->N_table, SINGLEMEM);
    textobj->N_table = -1;

    free(textobj->text_table);
}

//************************************
/// Reads the text from a file to program memory and returns it.
///
/// \param [in] char* name - name of the file to read from
/// \param [in] char* mode - a way how to read from the file: READ - to read normally as characters, READBYTES - to read text as byte sequence
///
///
/// \return pointer to the text from the file in program memory
///
//************************************

char* read_from_file (const char *name, const char* mode)
{
    int len_text = txt_file_length (name);

    FILE *txtfile = fopen (name, "r");
    if (!txtfile) return NULL;

    char* text_memory = (char*) calloc (len_text + 1, sizeof (*text_memory));

    if (fread (text_memory, sizeof (*text_memory), len_text, txtfile) <= 0) return NULL;

    fclose (txtfile);
    return text_memory;
}

//************************************
/// Gives a text file's length.
///
/// \param [in] char* name - name of the file
///
/// \return <int> length of the file
///
//************************************

int txt_file_length (const char *name)
{

    FILE *txtfile = fopen (name, "r");
    if (!txtfile) return 0;

    fseek (txtfile, 0, SEEK_END);
    int buf_len = ftell (txtfile);

    fclose (txtfile);
    return buf_len;
}

//************************************
/// Writes a text from MyText object to a file.
///
/// \param [in] char* name - name of the file to write
/// \param [in] MyText text - a pointer to MyText object to write from
/// \param [in] char* mode - a way how to write to the file: DELANDWRITE - to delete information in file and write normally as characters, \
        DELANDWRITEBYTES - to delete information in file and write text as byte sequence, \
        ADD - to add text to file normally as characters, \
        ADDBYTES- to add text to file as byte sequence,
///
/// \return 0 if writing was successful, 1 if not
///
//************************************

int write_Text_to_file (const char* name, MyText* mytext, const char* mode)
{
    FILE *sort_txt = fopen (name, mode);
    assert (sort_txt && mytext);

    for (size_t str = 0; str < mytext->N_table; ++str)
    {
        if (fputs (mytext->text_table[str], sort_txt) == EOF) return 1;
        fputc ('\n', sort_txt);
    }
    fputc ('\n', sort_txt);

    fclose (sort_txt);
    return 0;
}

//************************************
/// Writes a text from a table of strings to file.
///
/// \param [in] char* name - name of the file to write
/// \param [in] char** table - a pointer to a table of strings to write
/// \param [in] unsigned int N_table - number of strings to write
/// \param [in] char* mode - a way how to write to the file: DELANDWRITE - to delete information in file and write normally as characters, \
        DELANDWRITEBYTES - to delete information in file and write text as byte sequence, \
        ADD - to add text to file normally as characters, \
        ADDBYTES- to add text to file as byte sequence,
///
/// \return 0 if writing was successful, 1 if not
///
//************************************

int write_table_to_file (const char* name, char** table, size_t N_table, const char* mode)
{
    FILE *sort_txt = fopen (name, mode);
    assert (sort_txt && table);

    for (size_t str = 0; str < N_table; ++str)
    {
        if (table[str] && fputs (table[str], sort_txt) == EOF) return 1;
        fputc ('\n', sort_txt);
    }
    fputc ('\n', sort_txt);

    fclose (sort_txt);
    return 0;
}

//************************************
/// Writes a string to a file.
///
/// \param [in] char* name - name of the file to write
/// \param [in] char* txt - a pointer to string to write from
/// \param [in] unsigned int len - length of the string to write
/// \param [in] char* mode - a way how to write to the file: DELANDWRITE - to delete information in file and write normally as characters, \
        DELANDWRITEBYTES - to delete information in file and write text as byte sequence, \
        ADD - to add text to file normally as characters, \
        ADDBYTES- to add text to file as byte sequence,
///
/// \return 0 if writing was successful, 1 if not
///
//************************************

int write_to_file (const char* name, char* txt, size_t len, const char* mode)
{
    assert(name && txt && mode);

    FILE *sort_txt = fopen (name, mode);
    assert (sort_txt);

    if (fwrite(txt, sizeof(*txt), len, sort_txt) != len) return 1;

    fclose (sort_txt);
    return 0;
}

//************************************
/// Puts pointers of strings from a text to an array
///
/// \param [in] char* text - a pointer to the text
///
/// \return pointer to the array of string pointers
///
//************************************

char** get_text_in_table (char* text)
{
    assert(text);

    char* *table= (char**) calloc (string_counter (text) + 1, sizeof (*table));
    table[0] = text;

    int index = 1;
    for (char* strptr = strchr (text, '\n'); strptr; strptr = strchr (strptr + 1, '\n'))
    {
        *strptr = '\0';
        table[index++] = strptr + 1;
    }
    table[index] = NULL;

    return table;
}

//************************************
/// Reads the text from a file and puts it to Text object. The memory should be freed.
///
/// \param [in] char* name - the name of the file to  read from
/// \param [in] char* mode - a way how to read from the file: READ - to read normally as characters, READBYTES - to read text as byte sequence
///
/// \return pointer to the Text object
///
//************************************

MyText* read_in_Text (const char *name, const char* mode)
{
    assert(name && mode);

    MyText* text_pointer = (MyText*) calloc(1,sizeof(*text_pointer));

    char* file_text = read_from_file(name, mode);
    if (!file_text) return NULL;

    text_pointer->N_table = string_counter (file_text);
    text_pointer->text_table = get_text_in_table(file_text);


    return text_pointer;
}

//************************************
/// Counts the number of strings in a text.
///
/// \param [in] char* txtpointer - pointer to the text
///
///
/// \return the number of strings
///
//************************************


int string_counter (char* txtpointer)
{
    assert (txtpointer);

    int str_n = 0;
    for (char* strptr = strchr (txtpointer, '\n'); strptr; strptr = strchr (strptr + 1, '\n'))
    {
        str_n++;
    }

    return (str_n + 1);
}

//************************************
/// Frees text's and string pointers' memory
///
/// \param [in] char* *table - a pointer to an array with string pointers
/// \param [in] int n_table - a length of table[] array
/// \param [in] int mode - mode to free rows: SINGLEMEM if memory was single, OWNMEM if every row has its own memory
///
/// \return void
///
//************************************


void free_table (char** table, int n_table, int mode)
{
    assert(table);

    for (int i = 0; i < n_table; i++)
    {
        if (table[i])
        {
            memset (table[i], '\0', strlen(table[i]));
        }
        if (mode) { free(table[i]); table[i] = NULL;}
    }
    free(*table);
}
