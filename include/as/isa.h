#ifndef __ISA_H
#define __ISA_H

#include <stdint.h>

#include <emu/lc3.h>

struct instr_fmt
{
    const char mnemonic[8];
    enum lc3op opcode;
    lc3word operand_base;
    uint8_t operand_count;
    struct operand_fmt
    {
        enum
        {
            O_REG,
            O_IMM,
            O_ADDR
        } type;
        lc3word mask;
        uint8_t pos;
    } operands[3];
};

enum macro
{
    M_ASCII,
    M_BLKW,
    M_FILL,
    M_ORIGIN
};

struct macro_fmt
{
    const char mnemonic[8];
    enum macro macro_id;
    int8_t operand_count;
    enum
    {
        MO_IMM,
        MO_ASCII
    } operand_type;
};

extern const struct instr_fmt INSTR_TABLE[];
extern const struct macro_fmt MACRO_TABLE[];

int get_instr_table_size(void);
int get_macro_table_size(void);

#endif
