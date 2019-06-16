#ifndef __ISA_H
#define __ISA_H

#include <stdint.h>

#include <emu/lc3.h>

struct instruction_fmt
{
    const char mnemonic[8];
    enum lc3op opcode;
    lc3word operand_base;
    uint8_t operand_cnt;
    struct operand_fmt
    {
        enum operand_type
        {
            O_REG,
            O_IMM,
            O_ADDR
        } type;
        lc3word mask;
        uint8_t pos;
    } operands[3];
};

extern const struct instruction_fmt INSTRUCTION_TABLE[];

#endif
