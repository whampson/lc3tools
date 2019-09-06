#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <as/tokenizer.h>
#include <as/isa.h>

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

    // print_tokens(tok_list);

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
                printf("Instr: %s\n", ifmt->mnemonic);
                goto next;
            case T_LABEL_DEC:
                goto next;

        }

        if (operand_count <= 0 || ifmt == NULL)
        {
            printf("error: unexpected token - '%s'. Expecting instruction. (%d:%d)\n",
                curr->raw_str, curr->line, curr->pos);
            break;
        }

        ofmt = &ifmt->operands[ifmt->operand_cnt - operand_count];
        switch (ofmt->type)
        {
            case O_REG:
                if (curr->type != T_REGISTER)
                {
                    printf("error: unexpected token - '%s'. Expecting register. (%d:%d)\n",
                        curr->raw_str, curr->line, curr->pos);
                }
                break;
            case O_IMM:
                if (curr->type != T_LITERAL)
                {
                    printf("error: unexpected token - '%s'. Expecting immediate. (%d:%d)\n",
                        curr->raw_str, curr->line, curr->pos);
                }
                break;
            case O_ADDR:
                if (curr->type != T_LABEL_REF || curr->type != T_LITERAL)
                {
                    printf("error: unexpected token - '%s'. Expecting address. (%d:%d)\n",
                        curr->raw_str, curr->line, curr->pos);
                }
                break;
        }

        operand_count--;

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

    // lc3word instr;
    // const struct instruction_fmt *fmt;
    // const struct operand_fmt *opfmt;
    // int args[3] = { 6, 1, 5 };

    // instr = 0;
    // fmt = &INSTRUCTION_TABLE[23];   // RSHFL
    // opfmt = fmt->operands;

    // instr |= (fmt->opcode << 12) | fmt->operand_base;
    // for (int i = 0; i < fmt->operand_cnt; i++)
    // {
    //     instr |= (args[i] << opfmt[i].pos) & opfmt[i].mask;
    // }

    // printf("%4X\n", instr);

    return 0;
}
