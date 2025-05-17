#ifndef RECURSIVE_DESCENT_H_
#define RECURSIVE_DESCENT_H_

#include "token.h"

enum Node_type
{
    NUMBER = 1,
    PROCEDURE = 2,
    VARIABLE = 3,
    FUNC_2ARG = 4,
    FUNC_1ARG = 5,
    ERROR = 0 
};

enum Operations_type
{
    ADD       = 0,
    SUB       = 1,
    MUL       = 2,
    DIV       = 3,
    L_BRACKET = 4,
    R_BRACKET = 5,
    L_BRACE   = 6,
    R_BRACE   = 7,
    SEMICOLON = 8,
    COMMA     = 9,
    EQUAL     = 10,
    LESS      = 11,
    MORE      = 12,
    NOT_EQUAL = 13
};

enum Func_2arg_num
{
    INT       = 1,
    WHILE     = 2
};

enum Func_1arg_num
{
    PRINT_INT       = 1,
    PRINT_STR = 2,
    SCANF_INT = 3
};

union Node_value
{
    int number;
    int digit;
    char *name;
};

struct Node
{
    enum Node_type type;
    union Node_value value;
    struct Node    *left;
    struct Node    *right;
};

struct Operation 
{
    char  operation;
    int number;
};

struct Func
{
    char func [MAX_NAME_SIZE];
    int number;
};

const struct Operation operations [] = {  {'+', ADD},
                                    {'-', SUB}, 
                                    {'*', MUL}, 
                                    {'/', DIV}, 
                                    {'(', L_BRACKET},
                                    {')', R_BRACKET}, 
                                    {'{', L_BRACE}, 
                                    {'}', R_BRACE},
                                    {';', SEMICOLON},
                                    {',', COMMA},
                                    {'=', EQUAL},
                                    {'<', LESS}, 
                                    {'>', MORE},
                                    {'!', NOT_EQUAL}};

const struct Func func_2arg [] = {{"celoe", INT}};

                             
struct Node *node_ctor ();
void node_tree_dtor (struct Node *node);
struct Node *make_tree (struct Token **token_table, int *current_node);
struct Node *get_sum (struct Token **token_table, int *current_token);
struct Node *get_mul (struct Token **token_table, int *current_token);
struct Node *get_brackets (struct Token **token_table, int *current_token);
struct Node *get_num (struct Token **token_table, int *current_token);
struct Node *get_variable (struct Token **token_table, int *current_token);
struct Node *get_equality (struct Token **token_table, int *current_token);
struct Node *get_data (struct Token **token_table, int *current_token);
struct Node *get_print (struct Token **token_table, int *current_token);
struct Node *get_while (struct Token **token_table, int *current_token);
struct Node *get_compare (struct Token **token_table, int *current_token);
void node_tree_error (struct Token **token_table, int *current_token);
void treeDump (struct Node *root);
void treeDumpMakeNodeLabels (struct Node *root, int rang, FILE *dotFile);
void treeDumpMakeArrows (struct Node *root, FILE *dotFile);

#endif