#ifndef __CTOKENIZER_H
#define __CTOKENIZER_H

#include <as/CSourceFile.h>
#include <as/Token.h>

class CTokenizer
{
public:
    CTokenizer(CSourceFile& src);

    int read_token(Token& token);   // positive return indicates success

private:
    CSourceFile& m_src;
    char m_ch;

    bool eof() const;
    bool try_get_literal(std::string s, int *out) const;
    //bool get_ascii(std::string& out);
    char next_char();
    static bool is_delim(char ch);
    static bool is_valid(char ch);
};

#endif  /* __CTOKENIZER_H */
