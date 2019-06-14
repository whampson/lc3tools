#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <lc3as.h>
#include <lc3tools.h>

#define LINE_BUFFER_SIZE 256

struct source_file
{
    FILE *file;
    char line[LINE_BUFFER_SIZE];
    size_t line_len;
    size_t line_num;
    size_t line_pos;
};

int read_line(struct source_file *src)
{
    src->line_num++;
    if (feof(src->file) || fgets(src->line, LINE_BUFFER_SIZE, src->file) == 0)
    {
        return -1;
    }

    src->line_len = strlen(src->line);
    src->line_pos = 0;

    return src->line_len;
}

int get_opcode(char *mnemonic, int *opcode, int *operand_count)
{
    if (strcasecmp(mnemonic, "BR") == 0)
    {
        // TODO: nzp
        *opcode = 0x0;
        *operand_count = 1;
        return 1;
    }
    else if (strcasecmp(mnemonic, "ADD") == 0)
    {
        *opcode = 0x1;
        *operand_count = 3;
        return 1;
    }
    else if (strcasecmp(mnemonic, "RTI") == 0)
    {
        *opcode = 0x8;
        *operand_count = 0;
        return 1;
    }

    *operand_count = 0;
    return 0;
}

int main(int argc, char *argv[])
{
    struct source_file src;
    char *tok_head;
    char *tok_tail;
    char *mnemonic;
    char *label;
    char *operands[3];
    int opcode;
    int operand_count;
    int operand_idx;
    int label_seen;
    int mnemonic_seen;
    int non_whitespace_seen;
    int comment_seen;
    int valid_mnemonic;

    memset(&src, 0, sizeof(struct source_file));
    src.file = fopen(TEST_DIR"/astest2.asm", "rb");
    if (src.file == NULL)
    {
        return 2;
    }

    while (read_line(&src) != -1)
    {
        printf(">> line %zu...\n", src.line_num);

        operand_count = 0;
        operand_idx = 0;
        label_seen = 0;
        mnemonic_seen = 0;
        non_whitespace_seen = 0;
        comment_seen = 0;
        valid_mnemonic = 0;

        tok_head = src.line;
        tok_tail = tok_head;

        while (src.line_pos <= src.line_len)
        {
            switch (*tok_tail)
            {
                case '\r':
                {
                    goto next_char;
                }

                case ',':
                {
                    *tok_tail = '\0';
                    operands[operand_idx++] = tok_head;
                    tok_head = tok_tail + 1;
                    goto next_char;
                }

                case '\0':
                case '\n':
                case '\t':
                case ' ':
                {
                    if (comment_seen)
                    {
                        goto next_char;
                    }
                    if (!non_whitespace_seen)
                    {
                        tok_head++;
                        goto next_char;
                    }

                    non_whitespace_seen = 0;
                    *tok_tail = '\0';
                    if (!mnemonic_seen)
                    {
                        // Mnemonic
                        if (get_opcode(tok_head, &opcode, &operand_count))
                        {
                            mnemonic_seen = 1;
                            mnemonic = tok_head;
                        }
                    }
                    if (!comment_seen && mnemonic_seen && operand_idx == operand_count - 1)
                    {
                        // Operand
                        operands[operand_idx++] = tok_head;
                    }
                    if (!mnemonic_seen && !label_seen)
                    {
                        // Label
                        label_seen = 1;
                        label = tok_head;
                    }
                    tok_head = tok_tail + 1;
                    goto next_char;
                }

                case ';':
                {
                    comment_seen = 1;
                    goto next_char;
                }

                default:
                {
                    if (comment_seen)
                    {
                        goto next_char;
                    }

                    // TODO: restrict allowed characters
                    non_whitespace_seen = 1;
                    goto next_char;
                }
            }

        next_char:
            tok_tail++;
            src.line_pos++;
        }

    next_line:
        if (label_seen)
        {
            printf("LABEL = '%s'\n", label);
        }
        if (mnemonic_seen)
        {
            printf("MNEMONIC = '%s'\n", mnemonic);
        }
        for (int i = 0; i < operand_count; i++)
        {
            printf("OPERAND%d = '%s'\n", i, operands[i]);
        }
    }

    printf("Hit end of file! (line %zu)\n", src.line_num);
    fclose(src.file);

    return 0;
}
