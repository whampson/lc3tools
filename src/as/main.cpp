#include <cstdio>
#include <climits>
#include <fstream>
#include <vector>
#include <cstdarg>

#include <as/lc3as.h>
#include <as/CSourceFile.h>
#include <as/CLexer.h>
#include <as/Token.h>

int main(int argc, char *argv[])
{
    char src_path[PATH_MAX];
    int retval;

    if (argc < 2)
    {
        error("missing source file\n");
        return E_ARGUMENT;
    }

    realpath(argv[1], src_path);
    std::ifstream src_stream(src_path);
    if (src_stream.fail())
    {
        error("failed to open source file '%s'\n", argv[1]);
        return E_IO;
    }

    retval = 0;

    std::vector<Token> tokens;
    CSourceFile src(src_stream, src_path);
    CLexer tok(src);


    printf(" ln:pos\t\tlen\ttype\t\tval(dec)\tval(hex)\ttoken_str\n");

    Token t;
    while ((retval = tok.read_token(t)) > 0)
    {
        printf("% 3d:%d\t\t%zu\t", t.row, t.col, t.str_value.length());
        switch (t.type)
        {
            case TokenType::Identifier:
                printf("Identifier\t");
                break;
            case TokenType::Integer:
                printf("Integer\t\t");
                break;
            case TokenType::Ascii:
                printf("Ascii\t\t");
                break;
            case TokenType::Separator:
                printf("Separator\t");
                break;
        }

        printf("%d\t\t%4X\t\t'%s'\n",
            t.value, t.value & 0xFFFF, t.str_value.c_str());
    }

    return retval;
}

void error(const char *fmt, ...)
{
    va_list argp;
    va_start(argp, fmt);

    fprintf(stderr, "%s: error: ", LC3AS_EXE);
    vfprintf(stderr, fmt, argp);

    va_end(argp);
}

void error(const CSourceFile& src, const char *fmt, ...)
{
    va_list argp;
    va_start(argp, fmt);

    fprintf(stderr, "%s:%d:%d: error: ",
        src.path().c_str(), src.row(), src.col());
    vfprintf(stderr, fmt, argp);

    va_end(argp);
}

void error(const CSourceFile& src, int row, int col, const char *fmt, ...)
{
    va_list argp;
    va_start(argp, fmt);

    fprintf(stderr, "%s:%d:%d: error: ",
        src.path().c_str(), row, col);
    vfprintf(stderr, fmt, argp);

    va_end(argp);
}