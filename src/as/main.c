#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <as/tokenizer.h>
#include <as/isa.h>

#define E_ARGUMENT  1
#define E_IO        2
#define E_SYNTAX    3

#define DEFAULT_OUTFILE "a.out"
#define DEFAULT_ORIGIN  0x3000

struct symbol
{
    struct symbol *next;

    char name[64];
    lc3word addr;
};

void print_ifmt(const struct instr_fmt *ifmt)
{
    // menmonic op_fmt0 [op_fmt1 ...]
    printf("%s ", ifmt->mnemonic);
    for (int i = 0; i < ifmt->operand_count; i++)
    {
        switch (ifmt->operands[i].type)
        {
            case O_REG:  printf("O_REG ");  break;
            case O_IMM:  printf("O_IMMM "); break;
            case O_ADDR: printf("O_ADDR "); break;
        }
    }
    printf("\n");
}

int parse(struct source_file *src, struct token *tok_list)
{
    struct token *tok;
    lc3word pc;
    int retval;

    const struct instr_fmt *ifmt;
    const struct macro_fmt *mfmt;

    int parsing_instr;
    int parsing_macro;

    int op_idx;

    tok = tok_list;
    pc = DEFAULT_ORIGIN;
    retval = 0;

    parsing_instr = 0;
    parsing_macro = 0;

    // Pass 1: check syntax, determine program size, and load symbol table
    while (tok != NULL)
    {
        assert(!(parsing_instr == 1 && parsing_macro == 1));

        switch (tok->type)
        {
            // Instruction mnemonic
            case T_MNEMONIC_I:
            {
                parsing_instr = 1;
                parsing_macro = 0;
                op_idx = 0;
                ifmt = &INSTR_TABLE[tok->val];
                pc += 2;
                break;
            }

            // Macro mnemonic
            case T_MNEMONIC_M:
            {
                parsing_instr = 0;
                parsing_macro = 1;
                op_idx = 0;
                mfmt = &MACRO_TABLE[tok->val];
                break;
            }

            // Register name
            case T_REGISTER:
            {
                if (!parsing_instr || op_idx >= ifmt->operand_count || ifmt->operands[op_idx].type != O_REG)
                {
                    fprintf(stderr, "%s:%d:%d: error: unexpected register - 'R%d'\n",
                        src->path, tok->line, tok->pos, tok->val);
                    retval = E_ARGUMENT;
                    goto cleanup;
                }
                op_idx++;
                break;
            }

            // Label declaration
            case T_LABEL_DEC:
            {
                // TODO: add symbol to symbol table
                // printf("SYMBOL '%s' ADDRESS 0x%04x\n", tok->raw_str, pc);
                break;
            }

            // Label reference
            case T_LABEL_REF:
            {
                // TODO: look up address in symbol table
                break;
            }


        }

    next:
        tok = tok->next;
    }

cleanup:
    // TODO: clear symbol table

    return retval;
}

int main(int argc, char *argv[])
{
    int retval;

    struct source_file src;
    FILE *objfile;
    char *objname;

    struct token *tok_list;
    struct token *prev;
    struct token *curr;

    if (argc < 2)
    {
        fprintf(stderr, "%s: error: missing source file\n", argv[0]);
        return E_ARGUMENT;
    }

    memset(&src, 0, sizeof(struct source_file));
    realpath(argv[1], src.path);
    src.file = fopen(src.path, "rb");
    if (src.file == NULL)
    {
        fprintf(stderr, "%s: error: failed to open source file '%s'\n", argv[0], argv[1]);
        return E_IO;
    }

    objname = DEFAULT_OUTFILE;
    objfile = fopen(objname, "wb");
    if (objfile == NULL)
    {
        fprintf(stderr, "%s: error: failed to open output file '%s'\n", argv[0], objname);
        fclose(src.file);
        return E_IO;
    }

    retval = 0;

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

    print_tokens(tok_list);
    retval = parse(&src, tok_list);

    /* free token list */
    curr = tok_list;
    while (curr != NULL)
    {
        prev = curr;
        curr = curr->next;
        free(prev);
    }

    fclose(src.file);
    fclose(objfile);

    return retval;
}
