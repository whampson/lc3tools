#ifndef __CSOURCEFILE_H
#define __CSOURCEFILE_H

#include <fstream>
#include <string>

class CSourceFile
{
public:
    CSourceFile(std::ifstream& infile, const char * path);

    std::string path() const;       // full path of file
    int row() const;                // vert pos of last char read (1-indexed)
    int col() const;                // horiz pos of last char read (1-indexed)
    int marked_col() const;         // retrieve marked column number
    std::string marked_str() const; // retrieve marked string up to curr pos
    bool eof() const;               // has end-of-file been reached?

    void mark();                    // mark curr pos, resets with each new line
    char peek();                    // read char at curr pos
    char read();                    // read char at curr pos and advance

private:
    std::ifstream& m_infile;
    std::string m_path;
    std::string m_line;
    int m_row;
    int m_pos;                      // column; 0-indexed
    int m_mark;

    void next_line();
};

#endif  /* __CSOURCEFILE_H */
