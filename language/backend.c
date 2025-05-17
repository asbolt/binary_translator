#include "recursive_descent.h"

void change_variable_value (struct Node *node, FILE *file);
void asdfghj (struct Token **token_table, FILE *file);
void ariphmetic (struct Node *node, FILE *file);
void print_int (struct Node *node, FILE *file);
void scan_int (struct Node *node, FILE *file);
void print_str (struct Node *node, char *data_buffer, FILE *file);
char *data (struct Node *node, char *data_buffer);

int main ()
{
    char *buffer = read_file ("prog.txt");
    struct Token **token_table = make_token_table (buffer);
    token_table_dump (token_table);

    FILE *file = fopen ("text.txt", "w");
    asdfghj (token_table, file);
    token_table_dtor (token_table);
    fclose (file);
}

void asdfghj (struct Token **token_table, FILE *file)
{
    fprintf (file, "section .text\n \
                    global _start\n \
                    \n \
                    _start:\n");

    char *data_buffer = (char *)calloc (MAX_NAME_SIZE, sizeof (char));
    strcat (data_buffer, "section .data\n");

    struct Node *node = node_ctor();

    int current_token = 0;
    while ((node = make_tree (token_table, &current_token)) != NULL)
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
            
            case FUNC_2ARG:
                switch (node->value.number)
                {
                case INT:
                    data_buffer = data (node, data_buffer);
                    break;
                
                default:
                    break;
                }
            break;

            case FUNC_1ARG:
                switch (node->value.number)
                {
                case PRINT_INT:
                    print_int (node, file);
                    break;

                case PRINT_STR:
                    print_str (node, data_buffer, file);
                    break;

                case SCANF_INT:
                    scan_int (node, file);
                    break;
                
                default:
                    break;
                }
            break;
            
            default:
                break;
        }
    }

    fprintf (file, "mov rax, 60\n \
                    xor rdi, rdi\n \
                    syscall\n");

    fprintf (file, "%s", data_buffer);
    fprintf (file, "section .bss\n \
        buffer resb 20");
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

        fprintf (file, "pop rax\npop rbx\n");
        switch (node->value.number)
        {
            case ADD:
                fprintf (file, "add eax, ebx\npush rax\n");
                break;

            case SUB:
                fprintf (file, "sub eax, ebx\npush rax\n");
                break;

            case MUL:
                fprintf (file, "imul ebx\npush rax\n");
                break;

            case DIV:
                fprintf (file, "idiv ebx\npush rax\n");
                break;
            
            default:
                break;
        }
    } 
    else 
    {
        if (node->type == NUMBER)
        {
            fprintf (file, "mov eax, %d\npush rax\n", node->value.digit);
        } 
        else if (node->type == VARIABLE)
        {
            fprintf (file, "mov rax, 0\nmov eax, dword [%s]\npush rax\n", node->value.name);
        }
    }
}

char *data (struct Node *node, char *data_buffer)
{
    int old_size = strlen (data_buffer);

    data_buffer = (char *)realloc (data_buffer, old_size + 2 * MAX_NAME_SIZE);
    strcat (data_buffer, node->left->value.name);
    strcat (data_buffer, " dd ");

    int value = node->right->value.digit;
    char value_char[MAX_NAME_SIZE];
    int offset = MAX_NAME_SIZE + 1;

    if (value == 0)
    {
        strcat (data_buffer, "0");
    }

    while (value > 0)
    {
        value_char[offset] = value % 10 + '0';
        value = value / 10;
        offset--;
    }
    
    strcat (data_buffer, value_char + offset + 1);
    strcat (data_buffer, "\n");
    return data_buffer;
}


void print_int (struct Node *node, FILE *file)
{
    fprintf (file, "mov eax, dword [%s]\n \
mov ebx, 10\n \
mov rsi, buffer + 18\n \
next%p:\n \
mov edx, 0\n \
div ebx\n \
mov byte [rsi], dl\n \
add byte [rsi], '0'\n \
dec rsi\n \
cmp eax, 0 \n \
jne next%p\n \
inc rsi\n", node->left->value.name, node->left, node->left);


    fprintf (file, "mov rax, 1\nmov rdi, 1\nmov rdx, 20\n syscall\n");
}

void print_str (struct Node *node, char *data_buffer, FILE *file)
{
    int size = strlen (node->left->value.name);
    fprintf (file, "mov rsi, buffer\n");
    for (int symbol = 0; symbol < size; symbol++)
    {
        fprintf (file, "mov byte [rsi], %hhu\ninc rsi\n", node->left->value.name[symbol]);
    }

    fprintf (file, "mov rax, 1\nmov rdi, 1\nmov rsi, buffer\nmov rdx, %d\n syscall\n", size);
}


void scan_int (struct Node *node, FILE *file)
{
    fprintf (file, "mov rcx, 20\n \
    mov rdi, buffer \n \
    mov al, 0 \n \
    rep stosb\n");

    fprintf (file, "mov rax, 0\n \
    mov rdi, 0\n \
    mov rsi, buffer\n \
    mov rdx, 10\n \
    syscall\n");

    fprintf (file, "mov rdi, buffer\n \
                    mov ebx, 10\n \
                    mov eax, 0\n \
                    next%p:\n \
                    mul ebx\n \
                    mov edx, 0\n \
                    mov dl, byte [edi]\n \
                    add eax, edx\n \
                    sub eax, '0'\n \
                    inc edi\n \
                    cmp byte [edi], 10\n \
                    jne next%p\n", node, node);

    fprintf (file, "mov dword [%s], eax\n", node->left->value.name);

}