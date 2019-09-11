#ifndef __TOKEN_H
#define __TOKEN_H

#include <string>

#define DELIM_LABEL     ':'
#define DELIM_SEPARATOR ','
#define DELIM_ASCII     '"'
#define DELIM_COMMENT   ';'
#define DELIM_COMMENT2  '#'
#define DELIM_EOF       '\0'

#define PREFIX_MACRO    '.'
#define PREFIX_REG      'R'
#define PREFIX_SIGN     '-'

enum class TokenType
{
    Register,           // general purpose register,    example: R0
    Instruction,        // instruction mnemonic,        example: ADD
    Macro,              // macro mnemonic,              example: .FILL
    Separator,          // operand separator (,)
    Label,              // label declaration,           example: LABEL:
    Reference,          // label reference,             example: LABEL
    Literal,            // integer literal,             example: -3
    Ascii               // ASCII string literal,        example: "foo"
};

struct Token
{
    TokenType type;
    std::string str;    // string value (without delimiter)
    int value;          // integer value (not meaningfil for all types)
    int line;           // source file line number
    int pos;            // source file column number

    Token();
    std::string normalized() const;
};

#endif  /* __TOKEN_H */
