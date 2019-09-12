#include <as/CSourceFile.h>
#include <iostream>
#include <cstdio>

CSourceFile::CSourceFile(std::ifstream& infile, const char * path)
    : m_infile(infile)
    , m_path(path)
    , m_line("")
    , m_row(0)
    , m_pos(0)
    , m_mark(0)
{
    next_line();
}

std::string CSourceFile::path() const
{
    return m_path.c_str();
}

int CSourceFile::row() const
{
    return m_row;
}

int CSourceFile::col() const
{
    return m_pos + 1;
}

int CSourceFile::marked_col() const
{
    return m_mark + 1;
}

std::string CSourceFile::marked_str() const
{
    int len = m_pos - m_mark;
    if (len <= 0)
    {
        return "";
    }

    return m_line.substr(m_mark, len);
}

bool CSourceFile::eof() const
{
    return m_infile.eof() && (size_t) m_pos >= m_line.length();
}

void CSourceFile::mark()
{
    m_mark = m_pos;
}

char CSourceFile::peek()
{
    return m_line[m_pos];
}

char CSourceFile::read()
{
    char ch;

    if (eof() || m_infile.bad())
    {
        return '\0';
    }

    ch = m_line[m_pos];
    if (ch == '\n')
    {
        next_line();
    }
    else
    {
        m_pos++;
    }

    return ch;
}

void CSourceFile::next_line()
{
    if (m_infile.eof())
    {
        m_line = "";
        return;
    }

    std::getline(m_infile, m_line);

    m_line += "\n";
    m_row++;
    m_pos = 0;
    m_mark = 0;
}
