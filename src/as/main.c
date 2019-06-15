#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <lc3as.h>
#include <lc3tools.h>

#define LINE_BUFFER_SIZE    512
#define TOKEN_BUFFER_SIZE   64

#define ARRLEN(a)   (sizeof(a)/sizeof(a[0]))

enum token_type
{
    T_LABEL,
    T_OPCODE,
    T_PSEUDO_OP,
    T_REGISTER,
    T_IMMEDIATE
    // TODO: string literal
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

const char * const OPCODES[] =
{
    // NOTE: BR-BRnzp not included in this table
    "ADD",  "AND",  "JMP",  "JSR",  "JSRR", "LDB",  "LDW",  "LDI",
    "LEA",  "NOT",  "RET",  "RTI",  "LSHF", "RSHFL","RSHFA","STB",
    "STW",  "STI",  "TRAP", "XOR",

    "GETC", "HALT", "IN",   "OUT",  "PUTS", "PUTSP"
};

const char * const PSEUDO_OPS[] =
{
    ".BLKW",".END", ".FILL",".ORIG",".STRINGZ"
};

static struct token * read_token(struct source_file *src);
static int read_line(struct source_file *src);
static int try_read_constant(const char *s, int base, int *out);
static void s_toupper(char *s);
static int is_delim(char c);
static int is_opcode(const char *tok);

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
            case T_LABEL:
                printf("LABEL\t\t");
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
            case T_IMMEDIATE:
                printf("IMMEDIATE\t");
                break;
            default:
                printf("UNKNOWN\t");
                break;
        }

        printf("'%s'\n", curr->cap_str);

        prev = curr;
        curr = curr->next;
        free(prev);
    }

    return 0;
}

static struct token * read_token(struct source_file *src)
{
    struct token *token; /* I feel alright mamma I'm not jokin' */
    char *tok_head;
    char *tok_tail;
    int tok_seen;

    token = (struct token *) malloc(sizeof(struct token));
    if (token == NULL)
    {
        return NULL;
    }
    memset(token, 0, sizeof(struct token));


    if (src->line_pos >= src->line_len)
    {
    next_line:
        if (read_line(src) == -1)
        {
            printf("Hit end of file! (line %d)\n", src->line_num);
            free(token);
            return NULL;
        }
    }

start_over:
    tok_seen = 0;
    tok_head = &src->line[src->line_pos];
    tok_tail = tok_head;

    /* skip leading whitespace */
    while (isspace(*tok_head))
    {
        if (src->line_pos >= src->line_len)
        {
            goto next_line;
        }

        src->line_pos++;
        tok_head++;
    }

    tok_tail = tok_head;
    token->line = src->line_num;
    token->pos = src->line_pos + 1;

    /* read-in the token */
    while (!is_delim(*tok_tail))
    {
        tok_seen = 1;
        tok_tail++;
    }

    /* handle special delimiter cases */
    switch (*tok_tail)
    {
        case ',':
            if (!tok_seen)
            {
                src->line_pos++;
                goto start_over;
            }
            break;
        case ';':
        case '\0':
            goto next_line;
    }

    /* extract bookkeeping information */
    *tok_tail = '\0';
    token->len = (tok_tail - tok_head);
    src->line_pos += token->len + 1;
    strncpy(token->raw_str, tok_head, TOKEN_BUFFER_SIZE);
    strncpy(token->cap_str, tok_head, TOKEN_BUFFER_SIZE);
    s_toupper(token->cap_str);

    /* determine token type */
    if (is_opcode(token->cap_str))
    {
        token->type = T_OPCODE;
        return token;
    }
    switch (*tok_head)
    {
        case '.':
            token->type = T_PSEUDO_OP;
            break;
        case 'R':
            token->type = T_REGISTER;
            break;
        case '#':
        case 'x':
        case 'X':
            token->type = T_IMMEDIATE;
            break;
        default:
            token->type = T_LABEL;
            break;
    }

    return token;
}

static int read_line(struct source_file *src)
{
    src->line_num++;
    if (feof(src->file) || fgets(src->line, LINE_BUFFER_SIZE, src->file) == NULL)
    {
        return -1;
    }

    src->line_pos = 0;
    src->line_len = (int) strlen(src->line);

    return src->line_len;
}

static int try_read_constant(const char *s, int base, int *out)
{
    char *end;

    *out = strtol(s, &end, base);
    if (s == end)
    {
        return 0;
    }

    return 1;
}

static void s_toupper(char *s)
{
    while (*s != '\0')
    {
        *s = toupper(*s);
        s++;
    }
}

static int is_delim(char c)
{
    switch (c)
    {
        case ',':
        case ';':
        case ' ':
        case '\t':
        case '\r':
        case '\n':
        case '\0':
            return 1;
    }

    return 0;
}

static int is_opcode(const char *tok)
{
    int result;

    /* BR and friends */
    if (strncmp(tok, "BR", 2) == 0)
    {
        result= 1;
        tok += 2;

        while (*tok != '\0')
        {
            if (*tok != 'N' && *tok != 'Z' && *tok != 'P')
            {
                result = 0;
                break;
            }
            tok++;
        }

        return result;
    }

    /* all else */
    for (int i = 0; i < ARRLEN(OPCODES); i++)
    {
        if (strcmp(tok, OPCODES[i]) == 0)
        {
            return 1;
        }
    }

    return 0;
}
