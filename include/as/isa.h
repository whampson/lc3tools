#ifndef __ISA_H
#define __ISA_H

#include <stdint.h>
#include <types.h>

enum Macro
{
    M_ASCII,
    M_BLKW,
    M_FILL,
    M_ORIGIN
};


enum OperandType
{
    OT_IMM,
    OT_REG,
    OT_ADDR,
    OT_ASCII
};

struct InstructionFormat
{
    const char mnemonic[8];
    enum lc3op opcode;
    lc3word base;
    uint8_t count;
    struct OperandFormat
    {
        enum OperandType type;
        lc3word mask;
        uint8_t pos;
    } operands[3];
};

struct MacroFormat
{
    const char mnemonic[8];
    enum Macro id;
    int8_t count;
    enum OperandType type;
};

extern const struct InstructionFormat INSTR_TABLE[];
extern const struct MacroFormat MACRO_TABLE[];

int get_instr_table_size(void);
int get_macro_table_size(void);

#endif  // __ISA_H
