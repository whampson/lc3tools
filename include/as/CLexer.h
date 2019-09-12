#ifndef __CLEXER_H
#define __CLEXER_H

#include <as/CSourceFile.h>
#include <as/Token.h>

class CLexer
{
public:
    CLexer(CSourceFile& src);

    int read_token(Token& token);   // positive return indicates success

private:
    CSourceFile& m_src;

    bool try_parse_integer(const std::string& s, int *out_int) const;
    // bool try_parse_instr(const std::string& s, int *out_instr) const;
    // bool try_parse_macro(const std::string& s, int *out_macro) const;
    bool read_ascii(std::string& out_ascii);

    // bool seek_delim();
    bool seek_token();
    void seek_eol();

    static bool is_delim(char ch);
    static bool is_valid(char ch);
};

struct Lexeme
{

};

#endif  /* __CLEXER_H */
