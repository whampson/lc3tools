#ifndef __TOKEN_H
#define __TOKEN_H

#include <string>

#define DELIM_SEPARATOR1    ':'
#define DELIM_SEPARATOR2    ','
#define DELIM_ASCII         '"'
#define DELIM_COMMENT       ';'
#define DELIM_COMMENT_ALT   '#'

#define PREFIX_MACRO        '.'
#define PREFIX_REG          'R'
#define PREFIX_SIGN         '-'
#define PREFIX_ESCAPE       '\\'

#define ESCAPE_CR           'r'
#define ESCAPE_LF           'n'
#define ESCAPE_TAB          't'
#define ESCAPE_QUOTE        '"'
#define ESCAPE_BACKSLASH    '\\'

enum class TokenType
{
    // Register,           // general purpose register,    example: R0
    // Instruction,        // instruction mnemonic,        example: ADD
    // Macro,              // macro mnemonic,              example: .FILL
    // Separator,          // operand separator (,)
    // Label,              // label declaration,           example: LABEL:
    // Reference,          // label reference,             example: LABEL
    // Literal,            // integer literal,             example: -3
    // Ascii               // ASCII string literal,        example: "foo"

    Identifier,
    Integer,
    Ascii,
    Separator,
};

struct Token
{
    TokenType type;
    int value;
    std::string str_value;
    int row;
    int col;

    Token();
    std::string norm_str() const;   // normalized string value
};

#endif  /* __TOKEN_H */
