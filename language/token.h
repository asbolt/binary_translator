#ifndef TOKEN_H_
#define TOKEN_H_

#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <assert.h>
#include <stdlib.h>

#define MAX_NAME_SIZE 20

union Value
{
    char  operation;
    int digit;
    char  *string;
};

enum Token_type
{
    DIGIT     = 1,
    NAME      = 2,
    OPERATION = 3,
    EMPTY     = 0
};

struct Token 
{
    enum Token_type token_type;
    union Value     value;
};

char *read_file (const char *text_file);
struct Token *get_name (char *buffer, int *symbol_number);
struct Token *get_digit (char *buffer, int *symbol_number);
struct Token *get_operation (char *buffer, int *symbol_number);
struct Token **make_token_table (char *buffer);
struct Token **token_table_ctor (int spaces_amount);
struct Token *token_ctor ();
void token_table_dtor (struct Token **token_table);
void token_table_dump (struct Token **token_table);

#endif