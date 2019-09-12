#include <lc3tools.h>
#include <as/lc3as.h>
#include <as/isa.h>
#include <as/CLexer.h>

CLexer::CLexer(CSourceFile& src)
    : m_src(src)
{ }

#define CHK_VALID(ch)                                   \
({                                                      \
    if (!is_valid(ch))                                  \
    {                                                   \
        error(m_src, "syntax: unexpected '%c'\n", ch);  \
        return E_SYNTAX;                                \
    }                                                   \
})

int CLexer::read_token(Token& token)
{
    char ch;
    int row, col;

start_over:
    if (!seek_token())
    {
        return 0;
    }

    row = token.row = m_src.row();
    col = token.col = m_src.col();
    m_src.mark();

    CHK_VALID(m_src.peek());
    switch (ch = m_src.read())
    {
        case DELIM_COMMENT:
        case DELIM_COMMENT_ALT:
        {
            seek_eol();
            goto start_over;
        }
        case DELIM_SEPARATOR1:
        case DELIM_SEPARATOR2:
        {
            token.type = TokenType::Separator;
            token.value = 0;    // TODO: separator type enum
            token.str_value = m_src.marked_str();
            break;
        }
        case DELIM_ASCII:
        {
            token.type = TokenType::Ascii;
            token.value = 0;
            if (!read_ascii(token.str_value))
            {
                return E_SYNTAX;
            }
            break;
        }
        case PREFIX_SIGN:
        case '0':
        case '1':
        case '2':
        case '3':
        case '4':
        case '5':
        case '6':
        case '7':
        case '8':
        case '9':
        {
            while (!is_delim(m_src.peek()))
            {
                m_src.read();
            }
            token.type = TokenType::Integer;
            token.str_value = m_src.marked_str();
            if (!try_parse_integer(token.str_value, &token.value))
            {
                if (ch == PREFIX_SIGN)
                {
                    error(m_src, row, col, "expected a number after '%c'\n", PREFIX_SIGN);
                }
                else
                {
                    error(m_src, row, col, "not a number - '%s'\n", token.str_value.c_str());
                }
                return E_SYNTAX;
            }
            break;
        }
        default:
        {
            while (!is_delim(m_src.peek()))
            {
                CHK_VALID(m_src.peek());
                m_src.read();
            }
            token.type = TokenType::Identifier;
            token.value = 0;    // TODO: separator type enum
            token.str_value = m_src.marked_str();
            break;
        }
    }

    return 1;
}

bool CLexer::try_parse_integer(const std::string& s, int *out_int) const
{
    const char *str;
    char *end;
    size_t chars_read;

    str = s.c_str();
    *out_int = (int) strtol(str, &end, 0);
    chars_read = (end - str);

    return (str != end) && (chars_read == s.length());
}

bool CLexer::read_ascii(std::string& out_ascii)
{
    char ch;
    bool esc;
    int col;

    out_ascii = "";
    esc = false;

    while (esc || (m_src.peek() != DELIM_ASCII))
    {
        if (m_src.eof() || m_src.peek() == '\n')
        {
            error(m_src, m_src.row(), m_src.marked_col(),
                "unterminated ASCII string\n");
            return false;
        }

        col = m_src.col();
        ch = m_src.read();

        if (esc)
        {
            switch (ch)
            {
                case ESCAPE_CR:
                    ch = '\r';
                    break;
                case ESCAPE_LF:
                    ch = '\n';
                    break;
                case ESCAPE_TAB:
                    ch = '\t';
                    break;
                case ESCAPE_QUOTE:
                    ch = '"';
                    break;
                case ESCAPE_BACKSLASH:
                    ch = '\\';
                    break;
                default:
                    error(m_src, m_src.row(), col,
                        "invalid escape sequence - '%c'\n", ch);
                    return false;
            }
            esc = false;
        }
        else if (ch == PREFIX_ESCAPE)
        {
            esc = true;
            continue;
        }

        out_ascii.push_back(ch);
    }

    m_src.read();   // consume terminator
    return true;
}

bool CLexer::seek_token()
{
    while (isspace(m_src.peek()))
    {
        m_src.read();
        if (m_src.eof())
        {
            return false;
        }
    }

    return true;
}

void CLexer::seek_eol()
{
    while (m_src.peek() != '\n')
    {
        m_src.read();
        if (m_src.eof())
        {
            return;
        }
    }
}

bool CLexer::is_delim(char ch)
{
    switch (ch)
    {
        case DELIM_SEPARATOR1:
        case DELIM_SEPARATOR2:
        case DELIM_ASCII:
        case DELIM_COMMENT:
        case DELIM_COMMENT_ALT:
        case '\0':
            return true;
    }

    return isspace(ch);
}

bool CLexer::is_valid(char ch)
{
    return isalnum(ch)
        || is_delim(ch)
        || ch == PREFIX_MACRO
        || ch == PREFIX_SIGN
        || ch == '_';
}
