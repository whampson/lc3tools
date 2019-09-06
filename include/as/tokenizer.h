#ifndef __TOKENIZER_H
#define __TOKENIZER_H

#include <stdio.h>
#include <limits.h>

#define LINE_BUFFER_SIZE    1024

enum token_type
{
    T_LABEL_REF,    /* label reference */
    T_LABEL_DEC,    /* label declaration */
    T_MNEMONIC,     /* instruction mnemonic */
    T_MACRO,        /* macro mnemonic */
    T_REGISTER,     /* general purpose register */
    T_LITERAL,      /* integer literal */
    T_ASCII         /* ASCII string literal */  /* TODO: escape chars */
};

struct token
{
    struct token *next;

    int line;
    int pos;
    int len;

    enum token_type type;
    char raw_str[LINE_BUFFER_SIZE];
    char cap_str[LINE_BUFFER_SIZE];
    int val;
};

struct source_file
{
    FILE *file;
    char path[BUFSIZ];
    char line[LINE_BUFFER_SIZE];
    int line_num;
    int line_pos;
    int line_len;
};

struct token * read_token(struct source_file *src);
void print_tokens(const struct token *token_list);

#endif /* __TOKENIZER_H */
