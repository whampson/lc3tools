#include <assert.h>
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <lc3tools.h>
#include <as/lc3as.h>
#include <as/CTokenizer.h>

// TODO: port to C++

// static int try_read_constant(const char *s, int base, int *out);
static int get_mnemonic_number(const char *tok_str);
static int get_macro_number(const char *tok_str);

CTokenizer::CTokenizer(CSourceFile& src)
    : m_src(src)
    , m_ch('\0')
{ }

#define CHK_VALID(c)                                    \
({                                                      \
    if (!is_valid(c))                                   \
    {                                                   \
        error(m_src, "syntax: unexpected '%c'\n", c);   \
        return E_SYNTAX;                                \
    }                                                   \
})

int CTokenizer::read_token(Token& token)
{
    // probably has bugs


    std::string sym;
    std::string sym_norm;
    bool reading_ascii;

start_over:
    reading_ascii = false;

    if (eof())
    {
        return 0;
    }

    // trim leading whitespace
    while (isspace(m_ch) || m_src.line_num() < 1)
    {
        next_char();
        if (eof())
        {
            return 0;
        }
    }

    m_src.mark();
    token.value = 0;
    token.line = m_src.line_num();
    token.pos = m_src.line_pos();

find_delim:
    // read chars until a delimiter is found
    while (!is_delim(m_ch))
    {
        if (!reading_ascii)
        {
            CHK_VALID(m_ch);
        }
        next_char();
    }

    // ASCII: keep reading until terminating " found
    if (reading_ascii)
    {
        // TODO: escape chars

        switch (m_ch)
        {
            case DELIM_ASCII:
                // end of string
                break;
            case '\n':
            case DELIM_EOF:
                error(m_src, token, "syntax: unterminated ASCII string\n");
                return E_SYNTAX;
            default:
                // advance to next char
                next_char();
                goto find_delim;
        }

    }

    // process delimiter, determine token type
    switch (m_ch)
    {
        case DELIM_LABEL:
        {
            // TOOD: add to symbol table (?)
            if (m_src.marked().empty())
            {
                error(m_src, token, "syntax: unexpected ':'\n");
                return E_SYNTAX;
            }
            if (isdigit(sym_norm[0]))
            {
                error(m_src, token, "syntax: unexpected '%c'\n", sym_norm[0]);
                return E_SYNTAX;
            }
            token.str = m_src.marked();
            token.type = TokenType::Label;
            CHK_VALID(next_char());
            goto done;
        }

        case DELIM_SEPARATOR:
        {
            if (m_src.marked().empty())
            {
                CHK_VALID(next_char());
                token.str = m_src.marked();
                token.type = TokenType::Separator;
                goto done;
            }
            break;
        }

        case DELIM_ASCII:
        {
            if (!reading_ascii)
            {
                // begin reading ascii
                CHK_VALID(next_char());
                reading_ascii = true;
                m_src.mark();
                goto find_delim;
            }
            else
            {
                // end reading ASCII
                reading_ascii = false;
                token.str = m_src.marked();
                token.type = TokenType::Ascii;
                CHK_VALID(next_char());
                goto done;
            }
        }

        case DELIM_COMMENT:
        case DELIM_COMMENT2:
        {
            // skip comments
            m_src.advance();
            next_char();
            goto start_over;
        }
    }

    token.str = m_src.marked();
    sym = token.str;
    sym_norm = token.normalized();


    /* REGISTER */
    if (sym_norm[0] == PREFIX_REG)
    {
        int reg_num;
        if (try_get_literal(sym_norm.substr(1), &reg_num))
        {
            if (reg_num > 7)
            {
                error(m_src, token, "syntax: invalid register '%s'\n", sym.c_str());
                return E_SYNTAX;
            }
            token.type = TokenType::Register;
            token.value = reg_num;
            goto done;
        }
    }

    /* MACRO */
    if (sym_norm[0] == PREFIX_MACRO)
    {
        if (sym_norm.length() == 1)
        {
            error(m_src, token, "syntax: unexpected '%c'\n", PREFIX_MACRO);
            return E_SYNTAX;
        }

        token.type = TokenType::Macro;
        // TODO: get value
        goto done;
    }

    /* INSTRUCTION */
    // TODO: lookup

    /* LITERAL */
    {
        int literal;

        if (sym_norm[0] == PREFIX_SIGN && sym_norm.length() == 1)
        {
            error(m_src, token, "syntax: unexpected '%c'\n", PREFIX_SIGN);
            return E_SYNTAX;
        }

        if (try_get_literal(sym, &literal))
        {
            token.type = TokenType::Literal;
            token.value = literal;
            goto done;
        }
    }

    /* REFERENCE */
    {
        if (isdigit(sym_norm[0]))
        {
            error(m_src, token, "syntax: unexpected '%c'\n", sym_norm[0]);
            return E_SYNTAX;
        }
        token.type = TokenType::Reference;
        // TODO: lookup symbol in symbol table, set value to label address
    }

done:
    return 1;
}

bool CTokenizer::eof() const
{
    return m_src.eof();
}

bool CTokenizer::try_get_literal(std::string s, int *out) const
{
    const char *str;
    char *end;

    str = s.c_str();
    *out = (int) strtol(str, &end, 0);

    // printf("%s %s %zu %ld\n", str, end, s.length(), (end - str));

    return (str != end) && ((end - str) == (long) s.length());
}

char CTokenizer::next_char()
{
    m_ch = m_src.next_char();
    return m_ch;
}


bool CTokenizer::is_delim(char ch)
{
    switch (ch)
    {
        case DELIM_LABEL:
        case DELIM_SEPARATOR:
        case DELIM_ASCII:
        case DELIM_COMMENT:
        case DELIM_COMMENT2:
        case DELIM_EOF:
            return true;
    }

    return isspace(ch);
}

bool CTokenizer::is_valid(char ch)
{
    return isalnum(ch)
        || is_delim(ch)
        || ch == PREFIX_MACRO
        || ch == PREFIX_SIGN
        || ch == '_';
}