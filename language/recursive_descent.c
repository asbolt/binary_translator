#include "recursive_descent.h"

int main ()
{
    char *buffer = read_file ("prog.txt");
    struct Token **token_table = make_token_table (buffer);
    token_table_dump (token_table);

    struct Node *node = make_tree (token_table);
    token_table_dtor (token_table);
    treeDump (node);
    node_tree_dtor (node);
}

struct Node *node_ctor ()
{
    struct Node *node = (struct Node *)calloc (1, sizeof (struct Node));
    if (!node)
    {
        return NULL;
    }

    node->left = NULL;
    node->right = NULL;

    return node;
}

void node_tree_dtor (struct Node *node)
{
    if (node->left)
    {
        node_tree_dtor (node->left);
    }

    if (node->right)
    {
        node_tree_dtor (node->right);
    }

    if (node->type == VARIABLE)
    {
        free (node->value.name);
    }

    free (node);
}

struct Node *make_tree (struct Token **token_table)
{
    int current_node = 0;
    struct Node *node = get_sum (token_table, &current_node);
    return node;
}

struct Node *get_variable (struct Token **token_table, int *current_token)
{
    if (token_table[*current_token]->token_type == NAME)
    {
        struct Node *node = node_ctor();
        node->type = VARIABLE;

        node->value.name = (char *)calloc (MAX_NAME_SIZE, sizeof (char));
        if (!node->value.name)
        {
            return NULL;
        }
        memcpy (node->value.name, token_table[*current_token]->value.string, MAX_NAME_SIZE);

        (*current_token)++;
        return node;
    }
    else
    {
        node_tree_error (token_table, current_token);
        return NULL;
    }
}

struct Node *get_num (struct Token **token_table, int *current_token)
{
    if (token_table[*current_token]->token_type == DIGIT)
    {
        struct Node *node = node_ctor();
        node->type = NUMBER;
        node->value.digit = token_table[*current_token]->value.digit;
        (*current_token)++;
        return node;
    }
    else
    {
        return get_variable (token_table, current_token);
    }
}

void treeDump (struct Node *root)
{
    FILE * dotFile = fopen ("dot.dot", "wb");
        if (dotFile == NULL)
        {
            return;
        }

    fprintf (dotFile, "digraph G{\nrankdir=HR;\n");

    int rang = 0;

    treeDumpMakeNodeLabels (root, rang, dotFile);
    treeDumpMakeArrows (root, dotFile);

    fprintf (dotFile, "}");
    fclose (dotFile);

    system ("dot dot.dot -Tpng -o gr.png");
}

void treeDumpMakeNodeLabels (struct Node *root, int rang, FILE *dotFile)
{
    if (root->type == NUMBER)
    {
        fprintf (dotFile, "node%p [shape=\"rectangle\", label = \"%f\", rang = %d]\n", root, root->value.digit, rang);
    }
    else if (root->type == VARIABLE)
    {
        fprintf (dotFile, "node%p [shape=\"rectangle\", label = \"%s\", rang = %d]\n", root, root->value.name, rang);
    } 
    else if (root->type == PROCEDURE)
    {
        fprintf (dotFile, "node%p [shape=\"rectangle\", label = \"%c\", rang = %d]\n", root, operations[root->value.number].operation, rang);
    }
    
    if (root->left != NULL)
    {
        treeDumpMakeNodeLabels (root->left, rang + 1, dotFile);
    }

    if (root->right != NULL)
    {
        treeDumpMakeNodeLabels (root->right, rang + 1, dotFile);
    }

    return;
}

void treeDumpMakeArrows (struct Node *root, FILE *dotFile)
{
    if (root->left != NULL)
    {
        fprintf (dotFile, "node%p -> node%p\n", root, root->left);
        treeDumpMakeArrows (root->left, dotFile);
    }
    
    if (root->right != NULL)
    {
        fprintf (dotFile, "node%p -> node%p\n", root, root->right);
        treeDumpMakeArrows (root->right, dotFile);
    }
}

struct Node *get_sum (struct Token **token_table, int *current_token)
{
    struct Node *node = get_mul (token_table, current_token);

    if (token_table[*current_token]->token_type == OPERATION)
    { 
        while (token_table[*current_token]->value.operation == '+' || token_table[*current_token]->value.operation == '-')
        {
            if (token_table[*current_token]->value.operation == '+')
            {
                (*current_token)++;
                struct Node *parent_node = node_ctor();
                parent_node->type = PROCEDURE;
                parent_node->value.number = ADD;
                parent_node->left = node;
                parent_node->right = get_mul (token_table, current_token);

                node = parent_node;
            }
            else if (token_table[*current_token]->value.operation == '-')
            {
                (*current_token)++;
                struct Node *parent_node = node_ctor();
                parent_node->type = PROCEDURE;
                parent_node->value.number = SUB;
                parent_node->left = node;
                parent_node->right = get_mul (token_table, current_token);

                node = parent_node;
            }
        }
    }

    return node;
}

struct Node *get_mul (struct Token **token_table, int *current_token)
{
    struct Node *node = node_ctor ();
    node = get_brackets (token_table, current_token);

    if (token_table[*current_token]->token_type == OPERATION)
    {
        while (token_table[*current_token]->value.operation == '*' || token_table[*current_token]->value.operation == '/')
        {
            if (token_table[*current_token]->value.operation == '*')
            {
                (*current_token)++;
                struct Node *parent_node = node_ctor();
                parent_node->type = PROCEDURE;
                parent_node->value.number = MUL;
                parent_node->left = node;
                parent_node->right = get_brackets (token_table, current_token);

                node = parent_node;
            }
            else if (token_table[*current_token]->value.operation == '/')
            {
                (*current_token)++;
                struct Node *parent_node = node_ctor();
                parent_node->type = PROCEDURE;
                parent_node->value.number = DIV;
                parent_node->left = node;
                parent_node->right = get_brackets (token_table, current_token);

                node = parent_node;
            }
        }
    }

    return node;
}

struct Node *get_brackets (struct Token **token_table, int *current_token)
{
    assert (token_table);
    assert (current_token);

    if (token_table[*current_token]->token_type == OPERATION && token_table[*current_token]->value.operation == '(')
    {
        (*current_token)++;
        struct Node *node = node_ctor();
        node = get_sum (token_table, current_token);

        if (token_table[*current_token]->token_type == OPERATION && token_table[*current_token]->value.operation == ')')
        {
            (*current_token)++;
            return node;
        } else {
            node_tree_error (token_table, current_token);
            return NULL;
        }
    }
    else
    {
        return get_num (token_table, current_token);
    }
}

void node_tree_error (struct Token **token_table, int *current_token)
{
    if (token_table[*current_token]->token_type == DIGIT)
    {
        printf ("Error with token type %d: %f\n", token_table[*current_token]->token_type, token_table[*current_token]->value.digit);
    }
    else if (token_table[*current_token]->token_type == NAME)
    {
        printf ("Error with token type %d: %s\n", token_table[*current_token]->token_type, token_table[*current_token]->value.string);
    }
    else if (token_table[*current_token]->token_type == OPERATION)
    {
        printf ("Error with token type %d: %c\n", token_table[*current_token]->token_type, token_table[*current_token]->value.operation);
    }
}