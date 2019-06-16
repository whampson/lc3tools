#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <as/tokenizer.h>

int main(int argc, char *argv[])
{
    struct source_file src;

    struct token *tok_list;
    struct token *prev;
    struct token *curr;

    if (argc < 2)
    {
        fprintf(stderr, "error: missing source file\n");
        return 1;
    }

    memset(&src, 0, sizeof(struct source_file));
    src.file = fopen(argv[1], "rb");
    if (src.file == NULL)
    {
        fprintf(stderr, "error: failed to open source file\n");
        return 2;
    }

    /* build token list */
    prev = NULL;
    while ((curr = read_token(&src)) != NULL)
    {
        if (prev == NULL)
        {
            tok_list = curr;
            prev = tok_list;
            continue;
        }

        prev->next = curr;
        prev = curr;
    }

    /* output and free token list */
    printf(" ln:pos\tlen\ttype\t\ttoken\n");
    curr = tok_list;
    while (curr != NULL)
    {
        printf("% 3d:%d\t%d\t", curr->line, curr->pos, curr->len);
        switch (curr->type)
        {
            case T_LABEL_REF:
                printf("LABEL_REF\t");
                break;
            case T_LABEL_DEC:
                printf("LABEL_DEC\t");
                break;
            case T_OPCODE:
                printf("OPCODE\t\t");
                break;
            case T_PSEUDO_OP:
                printf("PSEUDO_OP\t");
                break;
            case T_REGISTER:
                printf("REGISTER\t");
                break;
            case T_LITERAL:
                printf("LITERAL\t\t");
                break;
            case T_ASCII:
                printf("ASCII\t\t");
                break;
        }

        printf("'%s'\n", curr->cap_str);

        prev = curr;
        curr = curr->next;
        free(prev);
    }

    return 0;
}
