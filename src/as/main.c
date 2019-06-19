#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <as/tokenizer.h>
#include <as/isa.h>

int main(int argc, char *argv[])
{
    // struct source_file src;

    // struct token *tok_list;
    // struct token *prev;
    // struct token *curr;

    // if (argc < 2)
    // {
    //     fprintf(stderr, "error: missing source file\n");
    //     return 1;
    // }

    // memset(&src, 0, sizeof(struct source_file));
    // src.file = fopen(argv[1], "rb");
    // if (src.file == NULL)
    // {
    //     fprintf(stderr, "error: failed to open source file\n");
    //     return 2;
    // }

    // /* build token list */
    // prev = NULL;
    // while ((curr = read_token(&src)) != NULL)
    // {
    //     if (prev == NULL)
    //     {
    //         tok_list = curr;
    //         prev = tok_list;
    //         continue;
    //     }

    //     prev->next = curr;
    //     prev = curr;
    // }

    // print_tokens(tok_list);

    // /* free token list */
    // curr = tok_list;
    // while (curr != NULL)
    // {
    //     prev = curr;
    //     curr = curr->next;
    //     free(prev);
    // }

    lc3word instr;
    const struct instruction_fmt *fmt;
    const struct operand_fmt *opfmt;
    int args[3] = { 6, 1, 5 };

    instr = 0;
    fmt = &INSTRUCTION_TABLE[23];   // RSHFL
    opfmt = fmt->operands;

    instr |= (fmt->opcode << 12) | fmt->operand_base;
    for (int i = 0; i < fmt->operand_cnt; i++)
    {
        instr |= (args[i] << opfmt[i].pos) & opfmt[i].mask;
    }

    printf("%4X\n", instr);

    return 0;
}
