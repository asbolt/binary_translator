#ifndef RECURSIVE_DESCENT_H_
#define RECURSIVE_DESCENT_H_

#include "token.h"

enum Node_type
{
    NUMBER = 1,
    PROCEDURE = 2,
    VARIABLE = 3,
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
    EQUAL     = 10
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
    float number;
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
                                    {'=', EQUAL}};

                             
struct Node *node_ctor ();
void node_tree_dtor (struct Node *node);
struct Node *make_tree (struct Token **token_table);
struct Node *get_sum (struct Token **token_table, int *current_token);
struct Node *get_mul (struct Token **token_table, int *current_token);
struct Node *get_brackets (struct Token **token_table, int *current_token);
struct Node *get_num (struct Token **token_table, int *current_token);
struct Node *get_variable (struct Token **token_table, int *current_token);
struct Node *get_equality (struct Token **token_table, int *current_token);
void node_tree_error (struct Token **token_table, int *current_token);
void treeDump (struct Node *root);
void treeDumpMakeNodeLabels (struct Node *root, int rang, FILE *dotFile);
void treeDumpMakeArrows (struct Node *root, FILE *dotFile);

#endif