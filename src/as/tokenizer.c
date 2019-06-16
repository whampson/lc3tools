#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <lc3tools.h>
#include <as/tokenizer.h>

const char * const OPCODES[] =
{
    "ADD",  "AND",  "BR",   "BRN",  "BRZ",  "BRP",  "BRNZ", "BRNP",
    "BRZP", "BRNZP","JMP",  "JSR",  "JSRR", "LDB",  "LDW",  "LDI",
    "LEA",  "NOT",  "RET",  "RTI",  "LSHF", "RSHFL","RSHFA","STB",
    "STW",  "STI",  "TRAP", "XOR",

    /* "GETC", "HALT", "IN",   "OUT",  "PUTS", "PUTSP" */
};

const char * const PSEUDO_OPS[] =
{
    ".ASCII", ".BLKW", ".FILL", ".ORIGIN", /* ".SEGMENT" */
};

static int read_line(struct source_file *src);
static int try_read_constant(const char *s, int base, int *out);
static void s_toupper(char *s);
static int is_delim(char c);
static int is_opcode(const char *tok);


struct token * read_token(struct source_file *src)
{
    struct token *token; /* I feel alright mamma I'm not jokin' */
    char *tok_head;
    char *tok_tail;
    int is_reading_tok;
    int is_reading_ascii;

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
    tok_head = &src->line[src->line_pos];
    tok_tail = tok_head;
    is_reading_tok = 0;
    is_reading_ascii = 0;

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
read_chars:
    while (!is_delim(*tok_tail))
    {
        is_reading_tok = 1;
        tok_tail++;
    }

    if (is_reading_ascii && *tok_tail != '"')
    {
        tok_tail++;
        goto read_chars;
    }

    /* handle special delimiter cases */
    switch (*tok_tail)
    {
        case ',':
            if (!is_reading_tok)
            {
                src->line_pos++;
                goto start_over;
            }
            break;
        case ':':
            token->type = T_LABEL_DEC;
            break;
        case '"':
            if (!is_reading_ascii)
            {
                token->type = T_ASCII;
                src->line_pos++;
                tok_head++;
                tok_tail++;
                is_reading_ascii = 1;
                goto read_chars;
            }
            break;
        case ';':
        case '\0':
            goto next_line;
    }

    /* extract token string and length */
    *tok_tail = '\0';
    token->len = (tok_tail - tok_head);
    strncpy(token->raw_str, tok_head, token->len);
    strncpy(token->cap_str, tok_head, token->len);
    s_toupper(token->cap_str);
    src->line_pos += token->len + 1;

    if (token->type == T_LABEL_DEC || token->type == T_ASCII)
    {
        goto done;
    }

    /* determine token type */
    if (is_opcode(token->cap_str))
    {
        token->type = T_OPCODE;
        goto done;
    }
    switch (toupper(*tok_head))
    {
        case '.':
            token->type = T_PSEUDO_OP;
            goto done;
        case 'R':
            token->type = T_REGISTER;
            goto done;
    }

    /* try conversion to integer */
    token->type = T_LITERAL;
    token->val = (int) strtol(tok_head, &tok_tail, 0);

    if (tok_head == tok_tail)
    {
        /* not an integer, assume label reference */
        token->type = T_LABEL_REF;
    }

done:
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
        case ',':   /* operand separator */
        case ';':   /* comment specifier */
        case ':':   /* label terminator */
        case '"':   /* string specifier  */
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
    for (int i = 0; i < ARRLEN(OPCODES); i++)
    {
        if (strcmp(tok, OPCODES[i]) == 0)
        {
            return 1;
        }
    }

    return 0;
}
