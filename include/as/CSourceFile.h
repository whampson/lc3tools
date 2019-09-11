#ifndef __CSOURCEFILE_H
#define __CSOURCEFILE_H

#include <fstream>
#include <string>

class CSourceFile
{
public:
    CSourceFile(std::ifstream& infile, const char * path);

    std::string path() const;
    int line_num() const;       // line number of last character read
    int line_pos() const;       // line postion of last character read
    bool eof() const;           // end-of-file reached

    char next_char();           // read next character
    void advance();             // advance to next line
    void mark();                // mark last character position
    std::string marked();       // retrieve marked string up to current position

private:
    std::ifstream& m_infile;
    std::string m_path;
    std::string m_line;
    int m_line_num;
    int m_line_pos;
    int m_mark;
    bool m_eol;
    bool m_eof;
};

#endif  /* __CSOURCEFILE_H */
