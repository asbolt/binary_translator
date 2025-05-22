#include <backend.h>

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