#include <as/CSourceFile.h>
#include <iostream>
#include <cstdio>

CSourceFile::CSourceFile(std::ifstream& infile, const char * path)
    : m_infile(infile)
    , m_path(path)
    , m_line("")
    , m_line_num(0)
    , m_line_pos(0)
    , m_eol(false)
    , m_eof(false)
{ }

std::string CSourceFile::path() const
{
    return m_path.c_str();
}

int CSourceFile::line_num() const
{
    return m_line_num;
}

int CSourceFile::line_pos() const
{
    return m_line_pos;
}

bool CSourceFile::eof() const
{
    return m_eof;
}

void CSourceFile::mark()
{
    m_mark = m_line_pos - 1;
}

std::string CSourceFile::marked()
{
    int len = (m_line_pos - 1) - m_mark;
    if (len <= 0)
    {
        return "";
    }

    return m_line.substr(m_mark, len);
}

char CSourceFile::next_char()
{
    char ch;

    if (m_eof)
    {
        return '\0';
    }
    else if (m_eol || m_line_num < 1)
    {
        advance();
        if (m_infile.bad())
        {
            return '\0';
        }
    }

    ch = m_line[m_line_pos];
    if (ch == '\n')
    {
        m_eol = true;
    }
    m_line_pos++;

    return ch;
}

void CSourceFile::advance()
{
    if (m_infile.eof())
    {
        m_eof = true;
        return;
    }

    std::getline(m_infile, m_line);

    m_line += "\n";
    m_eol = false;
    m_line_num++;
    m_line_pos = 0;
    m_mark = 0;
}