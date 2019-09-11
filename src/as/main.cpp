#include <cstdio>
#include <climits>
#include <fstream>
#include <vector>
#include <cstdarg>

#include <as/lc3as.h>
#include <as/CSourceFile.h>
#include <as/CTokenizer.h>
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
    CTokenizer tok(src);

    printf(" ln:pos\t\tlen\ttype\t\tval(dec)\tval(hex)\ttoken_str\n");

    Token t;
    while ((retval = tok.read_token(t)) > 0)
    {
        printf("% 3d:%d\t\t%zu\t", t.line, t.pos, t.str.length());
        switch (t.type)
        {
            case TokenType::Register:
                printf("Register\t");
                break;
            case TokenType::Instruction:
                printf("Instruction\t");
                break;
            case TokenType::Macro:
                printf("Macro\t\t");
                break;
            case TokenType::Separator:
                printf("Separator\t");
                break;
            case TokenType::Label:
                printf("Label\t\t");
                break;
                case TokenType::Reference:
                printf("Reference\t");
                break;
            case TokenType::Literal:
                printf("Literal\t\t");
                break;
            case TokenType::Ascii:
                printf("Ascii\t\t");
                break;
        }

        printf("%d\t\t%4X\t\t'%s'\n",
            t.value, t.value & 0xFFFF, t.normalized().c_str());
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
        src.path().c_str(), src.line_num(), src.line_pos());
    vfprintf(stderr, fmt, argp);

    va_end(argp);
}

void error(const CSourceFile& src, const Token& tok, const char *fmt, ...)
{
    va_list argp;
    va_start(argp, fmt);

    fprintf(stderr, "%s:%d:%d: error: ",
        src.path().c_str(), tok.line, tok.pos);
    vfprintf(stderr, fmt, argp);

    va_end(argp);
}