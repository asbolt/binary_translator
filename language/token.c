#include "token.h"

char *read_file (const char *text_file)
{
    assert (text_file);

    FILE *text = fopen (text_file, "r");
    if (!text)
    {
        return NULL;
    }

    fseek (text, 0, SEEK_END);
    size_t text_size = ftell (text);
    fseek (text, 0, SEEK_SET);

    char *buffer = (char *)calloc (text_size + 1, sizeof (char));
    if (!buffer)
    {
        fclose (text);
        return NULL;
    }

    fread (buffer, sizeof (char), text_size, text);
    buffer[text_size] = '\n';
    fclose (text);

    return buffer;
}

struct Token **make_token_table (char *buffer)
{
    assert (buffer);

    int symbol_number = 0;
    int buffer_size = strlen (buffer);

    struct Token **token_table = token_table_ctor (buffer_size);
    int current_token_number = 0;

    while (symbol_number < buffer_size)
    {
        if (buffer[symbol_number] == ' ' || buffer[symbol_number] == '\n')
        {
            symbol_number++;
        } 
        else if (isalpha (buffer[symbol_number]))
        {
            token_table[current_token_number] = get_name (buffer, &symbol_number);
            current_token_number++;
        } 
        else if (buffer[symbol_number] == '"')
        {
            token_table[current_token_number] = get_str (buffer, &symbol_number);
            current_token_number++;
        } 
        else if (isdigit (buffer[symbol_number]))
        {
            token_table[current_token_number] = get_digit (buffer, &symbol_number);
            current_token_number++;
        } 
        else 
        {
            token_table[current_token_number] = get_operation (buffer, &symbol_number);
            current_token_number++;
        }
    }

    token_table[current_token_number] = token_ctor ();

    return token_table;
}

struct Token **token_table_ctor (int spaces_amount)
{
    struct Token **token_table = (struct Token **)calloc (spaces_amount, sizeof (struct Token*));
    if (!token_table)
    {
        return NULL;
    }

    return token_table;
}

struct Token *token_ctor ()
{
    struct Token *token = (struct Token *)calloc (1, sizeof (struct Token*));
    if (!token)
    {
        return NULL;
    }

    token->token_type = EMPTY;
    return token;
}

void token_table_dtor (struct Token **token_table)
{
    assert (token_table);

    int amount_elements = 0;
    for (int element_number = 0; token_table[element_number]->token_type != EMPTY; element_number++) 
    {
        amount_elements++;
    }

    for (int element_number = 0; element_number <= amount_elements; element_number++)
    {
        if (token_table[element_number]->token_type == NAME)
        {
            free (token_table[element_number]->value.string);
        }

        free (token_table[element_number]);
    }

    free (token_table);
}

void token_table_dump (struct Token **token_table)
{
    assert (token_table);

    int node_number = 0;
    printf ("%10s%10s%20s\n", "number", "type", "value");

    while (token_table[node_number]->token_type != EMPTY)
    {
        if (token_table[node_number]->token_type == DIGIT)
        {
            printf ("%10d%10d%20d\n", node_number, token_table[node_number]->token_type, token_table[node_number]->value.digit);
            node_number++;
        }
        else if (token_table[node_number]->token_type == NAME)
        {
            printf ("%10d%10d%20s\n", node_number, token_table[node_number]->token_type, token_table[node_number]->value.string);
            node_number++;
        }
        else if (token_table[node_number]->token_type == OPERATION)
        {
            printf ("%10d%10d%20c\n", node_number, token_table[node_number]->token_type, token_table[node_number]->value.operation);
            node_number++;
        }
    }
}

struct Token *get_name (char *buffer, int *symbol_number)
{
    assert (buffer);
    assert (symbol_number);

    struct Token* token = token_ctor ();
    token->token_type = NAME;
    token->value.string = (char *)calloc (MAX_NAME_SIZE, sizeof (char));

    int element_number = 0;
    while (isalpha (buffer[*symbol_number]) || buffer[*symbol_number] == '_')
    {
        token->value.string[element_number] = buffer[*symbol_number];
        element_number++;
        (*symbol_number)++;
    }

    return token;
}

struct Token *get_digit (char *buffer, int *symbol_number)
{
    assert (buffer);
    assert (symbol_number);

    struct Token* token = token_ctor ();
    token->token_type = DIGIT;

    float number = 0;
    while (isdigit (buffer[*symbol_number]))
    {
        number = number*10 + buffer[*symbol_number] - '0';
        (*symbol_number)++;
    }

    if (buffer[*symbol_number] == ',')
    {
        (*symbol_number)++;

        while (isalpha (buffer[*symbol_number]))
        {
            number = (number * 10 + buffer[*symbol_number] - '0') / 10;
            (*symbol_number)++;
        }
    } 

    token->value.digit = number;
    return token;
}

struct Token *get_operation (char *buffer, int *symbol_number)
{
    assert (buffer);
    assert (symbol_number);

    struct Token* token = token_ctor ();
    token->token_type = OPERATION;
    token->value.operation = buffer[*symbol_number];
    (*symbol_number)++;
    return token;
}

struct Token *get_str (char *buffer, int *symbol_number)
{
    assert (buffer);
    assert (symbol_number);
    (*symbol_number)++;

    struct Token* token = token_ctor ();
    token->token_type = NAME;
    token->value.string = (char *)calloc (MAX_NAME_SIZE, sizeof (char));

    int element_number = 0;
    while (buffer[*symbol_number] != '"')
    {
        if (buffer[*symbol_number] == '\\')
        {
            token->value.string[element_number] = '\n';
            element_number++;
            (*symbol_number)++;
        }
        else
        {
            token->value.string[element_number] = buffer[*symbol_number];
            element_number++;
            (*symbol_number)++;
        }
    }

    (*symbol_number)++;

    return token;
}