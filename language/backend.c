#include "recursive_descent.h"

void change_variable_value (struct Node *node, FILE *file);
void asdfghj (struct Node *node, FILE *file);
void ariphmetic (struct Node *node, FILE *file);

int main ()
{
    char *buffer = read_file ("prog.txt");
    struct Token **token_table = make_token_table (buffer);
    token_table_dump (token_table);

    struct Node *node = make_tree (token_table);
    token_table_dtor (token_table);
    treeDump (node);

    FILE *file = fopen ("text.txt", "w");
    asdfghj (node, file);
    node_tree_dtor (node);
    fclose (file);
}

void asdfghj (struct Node *node, FILE *file)
{
    
    switch (node->type)
    {
        case PROCEDURE:
            switch (node->value.number)
            {
                case EQUAL:
                    ariphmetic (node->right, file);
                    fprintf (file, "mov [%s], eax\n", node->left->value.name);
                    break;
                
                default:
                    break;
            }
            break;
        
        default:
            break;
    }
}

void change_variable_value (struct Node *node, FILE *file)
{
    
}

void ariphmetic (struct Node *node, FILE *file)
{
    if (node->type == PROCEDURE)
    {
        ariphmetic (node->left, file);
        ariphmetic (node->right, file);

        fprintf (file, "pop eax\npop ebx\n");
        switch (node->value.number)
        {
            case ADD:
                fprintf (file, "add eax, ebx\npush eax\n");
                break;

            case SUB:
                fprintf (file, "sub eax, ebx\npush eax\n");
                break;

            case MUL:
                fprintf (file, "imul ebx\npush eax\n");
                break;

            case DIV:
                fprintf (file, "idiv ebx\npush eax\n");
                break;
            
            default:
                break;
        }
    } 
    else 
    {
        if (node->type == NUMBER)
        {
            fprintf (file, "mov eax, %d\npush eax\n", node->value.digit);
        } 
        else if (node->type == VARIABLE)
        {
            fprintf (file, "push dword [%s]\n", node->value.name);
        }
    }
}