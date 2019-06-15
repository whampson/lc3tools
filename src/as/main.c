#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <lc3as.h>
#include <lc3tools.h>

#define LINE_BUFFER_SIZE    512
#define TOKEN_BUFFER_SIZE   64

enum token_type
{
    T_LABEL,
    T_MNEMONIC,
    T_REGISTER,
    T_IMMEDIATE
};

struct token
{
    struct token *next;

    int line;
    int pos;
    int len;

    enum token_type type;
    char raw_str[TOKEN_BUFFER_SIZE];
    char cap_str[TOKEN_BUFFER_SIZE];
    int val;
};

struct source_file
{
    FILE *file;
    char line[LINE_BUFFER_SIZE];
    int line_num;
    int line_pos;
    int line_len;
};

static struct token * read_token(struct source_file *src);
static int read_line(struct source_file *src);

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

    if (read_line(&src) == -1)
    {
        fprintf(stderr, "error: failed to read source file\n");
        return 2;
    }

    prev = NULL;
    while ((curr = read_token(&src)) != NULL)
    {
        printf("(%d:%d, %d): ", curr->line, curr->pos, curr->len);
        switch (curr->type)
        {
            case T_LABEL:
                printf("<LABEL> %s\n", curr->raw_str);
                break;
            case T_MNEMONIC:
                printf("<MNEMONIC> %s\n", curr->cap_str);
                break;
            case T_REGISTER:
                printf("<REGISTER> %d\n", curr->val);
                break;
            case T_IMMEDIATE:
                printf("<IMMEDIATE> %d\n", curr->val);
                break;
            default:
                printf("<UNKNOWN> %s\n", curr->raw_str);
                break;
        }

        if (prev == NULL)
        {
            tok_list = curr;
            prev = tok_list;
        }
        else
        {
            prev->next = curr;
        }
        curr->next = NULL;
    }

    curr = tok_list;
    while (curr != NULL)
    {
        // printf("(%d, %d): ", curr->line, curr->pos);
        // switch (curr->type)
        // {
        //     case T_LABEL:
        //         printf("<LABEL> %s\n", curr->raw_str);
        //         break;
        //     case T_MNEMONIC:
        //         printf("<MNEMONIC> %s\n", curr->cap_str);
        //         break;
        //     case T_REGISTER:
        //         printf("<REGISTER> %d\n", curr->val);
        //         break;
        //     case T_IMMEDIATE:
        //         printf("<IMMEDIATE> %d\n", curr->val);
        //         break;
        //     default:
        //         printf("<UNKNOWN> %s\n", curr->raw_str);
        //         break;
        // }

        prev = curr;
        curr = curr->next;
        free(prev);
    }

    return 0;
}

int is_delim(char c)
{
    switch (c)
    {
        case ' ':
        case ',':
        case ';':
        case '\n':
        case '\0':
            return 1;
    }

    return 0;
}

static struct token * read_token(struct source_file *src)
{
    struct token *token; /* I feel alright mamma I'm not jokin' */
    char *tok_head;
    char *tok_tail;

    token = (struct token *) malloc(sizeof(struct token));
    if (token == NULL)
    {
        return NULL;
    }
    memset(token, 0, sizeof(struct token));

    tok_head = &src->line[src->line_pos];
    tok_tail = tok_head;

start:
    /* skip leading whitespace */
    while (isspace(*tok_head) || *tok_head == '\0')
    {
        if (src->line_pos >= src->line_len)
        {
            if (read_line(src) == -1)
            {
                printf("Hit end of file! (line %d)\n", src->line_num);
                free(token);
                return NULL;
            }
            tok_head = src->line;
            continue;
        }
        src->line_pos++;
        tok_head++;
    }

    tok_tail = tok_head;
    token->line = src->line_num;
    token->pos = src->line_pos + 1;

    while (!is_delim(*tok_tail))
    {
        tok_tail++;
    }

    // switch (*tok_tail)
    // {
    //     case '\0':
    //         src->line_pos = src->line_len;
    //         goto start;
    // }

    *tok_tail = '\0';
    token->len = (tok_tail - tok_head);
    token->type = T_LABEL;
    strcpy(token->raw_str, tok_head);
    src->line_pos += token->len;

    return token;
}

static int read_line(struct source_file *src)
{
    src->line_num++;
    if (feof(src->file) || fgets(src->line, LINE_BUFFER_SIZE, src->file) == NULL)
    {
        return -1;
    }

    // printf(">> %s", src->line);

    src->line_pos = 0;
    src->line_len = (int) strlen(src->line);

    return src->line_len;
}
