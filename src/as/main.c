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

int read_token(struct source_file *src, char **tok, char delim)
{
    size_t i;
    // char *tok;
    char *tok_head;
    char *tok_tail;
    char c;
    int char_seen;

    if (src->line_pos >= src->line_len)
    {
        return 0;
    }

    tok_head = &src->line[src->line_pos];
    tok_tail = tok_head;
    i = 0;
    char_seen = 0;

    while ((c = tok_head[i]) != '\0')
    {
        src->line_pos++;
        if (isspace(c))
        {
            if (!char_seen)
            {
                tok_head++;
                tok_tail++;
                continue;
            }
            else if (c == '\n')
            {
                *tok_tail = '\0';
                *tok = tok_head;
                return 1;
            }
            else if (!isspace(delim))
            {
                i++;
                continue;
            }
        }

        char_seen = 1;
        if (c == delim)
        {
            *tok_tail = '\0';
            *tok = tok_head;
            return 1;
        }
        i++;
        tok_tail++;
    }

    return 0;
}

int main(int argc, char *argv[])
{
    struct source_file src;
    char *tok;

    memset(&src, 0, sizeof(struct source_file));
    src.file = fopen(TEST_DIR"/astest2.asm", "rb");
    if (src.file == NULL)
    {
        return 2;
    }

    while (read_line(&src) != -1)
    {
        read_token(&src, &tok, ' ');
        // read_token(&src, &tok, '\t');

        printf("tok: %s\n", tok);
        if (strcasecmp(tok, "ADD") == 0)
        {
            printf("ADD\n");
            while ((read_token(&src, &tok, ',')) != 0)
            {
                printf("'%s'\n", tok);
            }
        }
    }

    printf("Hit end of file! (line %zu)\n", src.line_num);
    fclose(src.file);

    return 0;
}