#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <as/tokenizer.h>
#include <as/isa.h>

#define OUTFILE "a.out"

void print_ifmt(const struct instruction_fmt *ifmt)
{
    // menmonic op_fmt0 [op_fmt1 ...]
    printf("%s ", ifmt->mnemonic);
    for (int i = 0; i < ifmt->operand_cnt; i++)
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

int main(int argc, char *argv[])
{
    struct source_file src;
    FILE *objfile;
    char *objname;
    int retval;

    struct token *tok_list;
    struct token *prev;
    struct token *curr;

    retval = 0;

    if (argc < 2)
    {
        fprintf(stderr, "%s: error: missing source file\n", argv[0]);
        return 1;
    }

    memset(&src, 0, sizeof(struct source_file));
    realpath(argv[1], src.path);
    src.file = fopen(src.path, "rb");
    if (src.file == NULL)
    {
        fprintf(stderr, "%s: error: failed to open source file '%s'\n", argv[0], argv[1]);
        return 2;
    }

    objname = OUTFILE;

    objfile = fopen(objname, "wb");
    if (objfile == NULL)
    {
        fprintf(stderr, "%s: error: failed to open output file '%s'\n", argv[0], objname);
        fclose(src.file);
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

    print_tokens(tok_list);

    lc3word pc;
    const struct instruction_fmt *ifmt;
    const struct operand_fmt *ofmt;
    int operand_count;

    pc = 0;
    curr = tok_list;
    ifmt = NULL;
    operand_count = 0;

    while (curr != NULL)
    {
        switch (curr->type)
        {
            case T_MNEMONIC:
                ifmt = &INSTRUCTION_TABLE[curr->val];
                operand_count = ifmt->operand_cnt;
                goto next;
            case T_LABEL_DEC:
                // TODO: add to symbol table
                goto next;
            case T_MACRO:
            case T_ASCII:
                // TODO: process macro
                goto next;
            default:
                break;
        }

        if (operand_count <= 0 || ifmt == NULL)
        {
            printf("%s:%d:%d: error: expected instruction, label, or macro - '%s'\n",
                src.path, curr->line, curr->pos, curr->raw_str);
            break;
        }

        const char *mnemonic = ifmt->mnemonic;
        int error = 0;
        int found = 0;

        while ((error = strcmp(mnemonic, ifmt->mnemonic)) == 0 && !found)
        {
            ofmt = &ifmt->operands[ifmt->operand_cnt - operand_count];
            switch (ofmt->type)
            {
                case O_REG:
                    if (curr->type != T_REGISTER)
                    {
                        ifmt++;
                    }
                    else
                    {
                        found = 1;
                    }
                    break;
                case O_IMM:
                    if (curr->type != T_LITERAL)
                    {
                        ifmt++;
                    }
                    else
                    {
                        found = 1;
                    }
                    break;
                case O_ADDR:
                    if (curr->type != T_LABEL_REF && curr->type != T_LITERAL)
                    {
                        ifmt++;
                    }
                    else
                    {
                        found = 1;
                    }
                    break;
            }
        }

        operand_count--;

        if (error)
        {
            printf("%s:%d:%d: error: invalid syntax - '%s'.\n",
                src.path, curr->line, curr->pos, curr->raw_str);
        }
        else if (operand_count == 0)
        {
            // Assemble!
            lc3word instr;
            ofmt = ifmt->operands;

            instr = (ifmt->opcode << 12) | ifmt->operand_base;
            for (int i = 0; i < ifmt->operand_cnt; i++)
            {
                instr |= (curr->val << ofmt[i].pos) & ofmt[i].mask;
            }

            if (fwrite(&instr, sizeof(lc3word), 1, objfile) == 0)
            {
                fprintf(stderr, "%s: error: encountered a problem while writing output file '%s'\n", argv[0], objname);
                retval = 3;
                break;
            }

        }

    next:
        prev = curr;
        curr = curr->next;
    }

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
