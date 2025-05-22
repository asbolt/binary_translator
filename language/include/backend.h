#ifndef BACKEND_H_
#define BACKEND_H_

#include "recursive_descent.h"

enum Status
{
    LOKAL = 1,
    PASSED = 2
};

struct Lokal_var 
{
    char** names;
    int *numbers;
    int *status;
};

void get_usr_func (struct Node *node, FILE *file);
void asdfghj (struct Token **token_table, FILE *file);
char *jhvljvl (struct Token **token_table, FILE *file, char *data_buffer, int *current_token, struct Node *node, const char *function);
void ariphmetic (struct Node *node, FILE *file, const char *function);
void print_int (struct Node *node, FILE *file, const char *function);
void scan_int (struct Node *node, FILE *file, const char *function);
void get_while_asm (struct Node *node, FILE *file, char *data_buffer, struct Token **token_table, int *current_token, const char *function);
void get_if (struct Node *node, FILE *file, char *data_buffer, struct Token **token_table, int *current_token, const char *function);
char *get_new_func (struct Node *node, FILE *file, char *data_buffer, struct Token **token_table, int *current_token);
void print_str (struct Node *node, char *data_buffer, FILE *file);
char *data (struct Node *node, char *data_buffer, const char *function);
void calling_func (struct Node *node, FILE *file, struct Token **token_table, int *current_token, const char *function);

#endif