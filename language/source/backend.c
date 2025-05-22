#include "backend.h"

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
        data_buffer = jhvljvl (token_table, file, data_buffer, &current_token, node, "main");
    }

    fprintf (file, "%s", data_buffer);
    fprintf (file, "section .bss\n \
        buffer resb 20");
}

void ariphmetic (struct Node *node, FILE *file, const char *function)
{
    if (node->type == PROCEDURE)
    {
        ariphmetic (node->left, file, function);
        ariphmetic (node->right, file, function);

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
            fprintf (file, "mov rax, 0\nmov eax, dword [%s%s]\npush rax\n", node->value.name, function);
        }
    }
}

char *data (struct Node *node, char *data_buffer, const char *function)
{
    int old_size = strlen (data_buffer);

    data_buffer = (char *)realloc (data_buffer, old_size + 2 * MAX_NAME_SIZE);
    strcat (data_buffer, node->left->value.name);
    strcat (data_buffer, function);
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


void print_int (struct Node *node, FILE *file, const char *function)
{
    fprintf (file, "mov eax, dword [%s%s]\n \
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
inc rsi\n", node->left->value.name, function, node->left, node->left);


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


void scan_int (struct Node *node, FILE *file, const char *function)
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

    fprintf (file, "mov dword [%s%s], eax\n", node->left->value.name, function);

}

void get_while_asm (struct Node *node, FILE *file, char *data_buffer, struct Token **token_table, int *current_token, const char *function)
{
    (*current_token)++;

    fprintf (file, "jmp while%p\ncond%p:\n", node, node);

    struct Node *node_while = node_ctor ();
    while (token_table[*current_token]->token_type != OPERATION && token_table[*current_token]->value.operation != '}')
    {
        node_while = make_tree (token_table, current_token);
        jhvljvl (token_table, file, data_buffer, current_token, node_while, function);
    }

    (*current_token)++;

    fprintf (file, "while%p:\n", node);
    ariphmetic (node->left->left, file, function);
    fprintf (file, "mov ecx, eax\n");
    ariphmetic (node->left->right, file, function);
    fprintf (file, "mov ebx, ecx\n\
                    cmp ebx, eax\n");

    switch (node->left->value.number)
    {
    case LESS:
        fprintf (file, "jl cond%p\n", node);
        break;

    case MORE:
        fprintf (file, "jg cond%p\n", node);
        break;

    case EQUAL:
        fprintf (file, "je cond%p\n", node);
        break;

    case NOT_EQUAL:
        fprintf (file, "jne cond%p\n", node);
        break;
    
    default:
        break;
    }
}

char *jhvljvl (struct Token **token_table, FILE *file, char *data_buffer, int *current_token, struct Node *node, const char *function)
{
    switch (node->type)
        {
            case PROCEDURE:
                switch (node->value.number)
                {
                    case EQUAL:
                        if (node->right->type == FUNC_1ARG)
                        {
                            calling_func (node->right, file, token_table, current_token, function);
                        }
                        else 
                        {
                            ariphmetic (node->right, file, function);
                        }
                        fprintf (file, "mov [%s%s], eax\n", node->left->value.name, function);
                        break;

                    default:
                        break;
                }
            break;
            
            case FUNC_2ARG:
                switch (node->value.number)
                {
                case INT:
                    data_buffer = data (node, data_buffer, function);
                    break;

                case WHILE:
                    get_while_asm (node, file, data_buffer, token_table, current_token, function);
                    break;

                case IF:
                    get_if (node, file, data_buffer, token_table, current_token, function);
                    break;

                case FUNC:
                    data_buffer = get_new_func (node, file, data_buffer, token_table, current_token);
                    break;
                
                default:
                    break;
                }
            break;

            case FUNC_1ARG:
                switch (node->value.number)
                {
                case PRINT_INT:
                    print_int (node, file, function);
                    break;

                case PRINT_STR:
                    print_str (node, data_buffer, file);
                    break;

                case SCANF_INT:
                    scan_int (node, file, function);
                    break;

                case EXIT:
                    fprintf (file, "mov rax, 60\n \
                    xor rdi, rdi\n \
                    syscall\n");
                    break;

                case CALL:
                    calling_func (node, file, token_table, current_token, function);
                    break;  
                
                default:
                    break;
                }
            break;
            
            default:
                break;
        }

    return data_buffer;
}

void get_if (struct Node *node, FILE *file, char *data_buffer, struct Token **token_table, int *current_token, const char *function)
{
    (*current_token)++;

    fprintf (file, "jmp if%p\ncond%p:\n", node, node);

    struct Node *node_while = node_ctor ();
    while (token_table[*current_token]->token_type != OPERATION && token_table[*current_token]->value.operation != '}')
    {
        node_while = make_tree (token_table, current_token);
        jhvljvl (token_table, file, data_buffer, current_token, node_while, function);
    }

    fprintf (file, "jmp end%p\n", node);
    fprintf (file, "else%p:\n", node);

    (*current_token)++;

    if (token_table[*current_token]->token_type == NAME && strcmp (token_table[*current_token]->value.string, "inache") == 0)
    {
        (*current_token) += 2;

        struct Node *node_else = node_ctor ();
        while (token_table[*current_token]->token_type != OPERATION && token_table[*current_token]->value.operation != '}')
        {
            node_else = make_tree (token_table, current_token);
            jhvljvl (token_table, file, data_buffer, current_token, node_else, function);
        }

        fprintf (file, "jmp end%p\n", node);
        (*current_token)++;

    }

    fprintf (file, "if%p:\n", node);
    ariphmetic (node->left->left, file, function);
    fprintf (file, "mov ecx, eax\n");
    ariphmetic (node->left->right, file, function);
    fprintf (file, "mov ebx, ecx\n\
                    cmp ebx, eax\n");

    switch (node->left->value.number)
    {
    case LESS:
        fprintf (file, "jl cond%p\n", node);
        break;

    case MORE:
        fprintf (file, "jg cond%p\n", node);
        break;

    case EQUAL:
        fprintf (file, "je cond%p\n", node);
        break;

    case NOT_EQUAL:
        fprintf (file, "jne cond%p\n", node);
        break;
    
    default:
        break;
    }

    fprintf (file, "jmp else%p\n", node);
    fprintf (file, "end%p:\n", node);
}

char *get_new_func (struct Node *node, FILE *file, char *data_buffer, struct Token **token_table, int *current_token)
{
    fprintf (file, "%s:\npush rbp\nmov rbp, rsp\n", node->left->value.name);
    int arg_number = 0;
    (*current_token)++;

    while (token_table[*current_token]->token_type == NAME)
    {
        struct Node *node_name = node_ctor();
        struct Node *left = node_ctor();
        struct Node *right = node_ctor();

        left->value.name = (char *)calloc (MAX_NAME_SIZE, sizeof (char));
        strcpy (left->value.name, token_table[*current_token]->value.string);
        right->value.digit = 0;
        node_name->left = left;
        node_name->right = right;
        data_buffer = data (node_name, data_buffer, node->left->value.name);
        (*current_token) += 2;
        fprintf (file, "mov eax, dword [rbp + %d]\nmov dword [%s%s], eax\n", 16 + arg_number*8, node_name->left->value.name, node->left->value.name);
        arg_number++;
    }

    if (token_table[*current_token]->token_type != OPERATION && token_table[*current_token]->value.operation != ')')
    {
        (*current_token)++;
    }

    struct Node *node_func = node_ctor ();
    (*current_token)++;
    while (!(token_table[*current_token]->token_type == NAME && strcmp (token_table[*current_token]->value.string, "vernut") == 0))
    {
        node_func = make_tree (token_table, current_token);
        jhvljvl (token_table, file, data_buffer, current_token, node_func, node->left->value.name);
    }
    (*current_token)++;

    if (token_table[*current_token]->token_type == NAME)
    {
        fprintf (file, "mov eax, dword [%s%s]\n", token_table[*current_token]->value.string, node->left->value.name);
        (*current_token)++;
    }
    (*current_token) += 2;

    fprintf (file, "mov rsp, rbp\npop rbp\nret %d\n", arg_number * 8);

    return data_buffer;
}

void calling_func (struct Node *node, FILE *file, struct Token **token_table, int *current_token, const char *function)
{
    while (!(token_table[*current_token]->token_type == OPERATION && token_table[*current_token]->value.operation == ')'))
    {
        (*current_token)++;
    }

    int after_pos = *current_token;
    (*current_token)--;

    while (!(token_table[*current_token]->token_type == OPERATION && token_table[*current_token]->value.operation == '('))
    {
        fprintf (file, "mov eax, dword [%s%s]\npush rax\n", token_table[*current_token]->value.string, function);
        (*current_token)--;

        if (token_table[*current_token]->token_type == OPERATION && token_table[*current_token]->value.operation == ',')
        {
            (*current_token)--;
        }
    }

    *current_token = after_pos + 2;

    fprintf (file, "call %s\n", node->left->value.name);
}