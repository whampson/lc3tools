#include <lc3tools.h>
#include <as/isa.h>

const struct instr_fmt INSTR_TABLE[] =
{
    { "ADD",    OP_ADD, 0x000, 3, {{ O_REG, 0xE00, 9 },{ O_REG, 0x1C0, 6 },{ O_REG, 0x007, 0 }} },
    { "ADD",    OP_ADD, 0x020, 3, {{ O_REG, 0xE00, 9 },{ O_REG, 0x1C0, 6 },{ O_IMM, 0x01F, 0 }} },
    { "AND",    OP_AND, 0x000, 3, {{ O_REG, 0xE00, 9 },{ O_REG, 0x1C0, 6 },{ O_REG, 0x007, 0 }} },
    { "AND",    OP_AND, 0x020, 3, {{ O_REG, 0xE00, 9 },{ O_REG, 0x1C0, 6 },{ O_IMM, 0x01F, 0 }} },
    { "BR",     OP_BR,  0xE00, 1, {{ O_ADDR,0x1FF, 0 }                                        } },
    { "BRN",    OP_BR,  0x800, 1, {{ O_ADDR,0x1FF, 0 }                                        } },
    { "BRZ",    OP_BR,  0x400, 1, {{ O_ADDR,0x1FF, 0 }                                        } },
    { "BRP",    OP_BR,  0x200, 1, {{ O_ADDR,0x1FF, 0 }                                        } },
    { "BRNZ",   OP_BR,  0xC00, 1, {{ O_ADDR,0x1FF, 0 }                                        } },
    { "BRNP",   OP_BR,  0xA00, 1, {{ O_ADDR,0x1FF, 0 }                                        } },
    { "BRZP",   OP_BR,  0x600, 1, {{ O_ADDR,0x1FF, 0 }                                        } },
    { "BRNZP",  OP_BR,  0xE00, 1, {{ O_ADDR,0x1FF, 0 }                                        } },
    { "JMP",    OP_JMP, 0x000, 1, {{ O_REG, 0x1C0, 6 }                                        } },
    { "JSR",    OP_JSR, 0x800, 1, {{ O_ADDR,0x7FF, 0 }                                        } },
    { "JSRR",   OP_JSR, 0x000, 1, {{ O_REG, 0x1C0, 6 }                                        } },
    { "LDB",    OP_LDB, 0x000, 3, {{ O_REG, 0xE00, 9 },{ O_REG, 0x1C0, 6 },{ O_ADDR,0x03F, 0 }} },
    { "LDW",    OP_LDW, 0x000, 3, {{ O_REG, 0xE00, 9 },{ O_REG, 0x1C0, 6 },{ O_ADDR,0x03F, 0 }} },
    { "LDI",    OP_LDI, 0x000, 3, {{ O_REG, 0xE00, 9 },{ O_REG, 0x1C0, 6 },{ O_ADDR,0x03F, 0 }} },
    { "LEA",    OP_LEA, 0x000, 2, {{ O_REG, 0xE00, 9 },{ O_ADDR,0x1FF, 0 }                    } },
    { "NOT",    OP_XOR, 0x03F, 2, {{ O_REG, 0xE00, 9 },{ O_REG, 0x1C0, 6 }                    } },
    { "RET",    OP_JMP, 0x1C0, 0,                                                               },
    { "RTI",    OP_RTI, 0x000, 0,                                                               },
    { "LSHF",   OP_SHF, 0x000, 3, {{ O_REG, 0xE00, 9 },{ O_REG, 0x1C0, 6 },{ O_IMM, 0x00F, 0 }} },
    { "RSHFL",  OP_SHF, 0x010, 3, {{ O_REG, 0xE00, 9 },{ O_REG, 0x1C0, 6 },{ O_IMM, 0x00F, 0 }} },
    { "RSHFA",  OP_SHF, 0x030, 3, {{ O_REG, 0xE00, 9 },{ O_REG, 0x1C0, 6 },{ O_IMM, 0x00F, 0 }} },
    { "STB",    OP_STB, 0x000, 3, {{ O_REG, 0xE00, 9 },{ O_REG, 0x1C0, 6 },{ O_ADDR,0x03F, 0 }} },
    { "STW",    OP_STW, 0x000, 3, {{ O_REG, 0xE00, 9 },{ O_REG, 0x1C0, 6 },{ O_ADDR,0x03F, 0 }} },
    { "STI",    OP_STI, 0x000, 3, {{ O_REG, 0xE00, 9 },{ O_REG, 0x1C0, 6 },{ O_ADDR,0x03F, 0 }} },
    { "TRAP",   OP_TRAP,0x000, 1, {{ O_ADDR,0x0FF, 0 }                                        } },
    { "XOR",    OP_XOR, 0x000, 3, {{ O_REG, 0xE00, 9 },{ O_REG, 0x1C0, 6 },{ O_REG, 0x007, 0 }} },
    { "XOR",    OP_XOR, 0x020, 3, {{ O_REG, 0xE00, 9 },{ O_REG, 0x1C0, 6 },{ O_IMM, 0x01F, 0 }} },

 /* { "GETC",   OP_TRAP,0x020, 0,                                                               }, */
 /* { "OUT",    OP_TRAP,0x021, 0,                                                               }, */
 /* { "PUTS",   OP_TRAP,0x022, 0,                                                               }, */
 /* { "IN",     OP_TRAP,0x023, 0,                                                               }, */
 /* { "PUTSP",  OP_TRAP,0x024, 0,                                                               }, */
 /* { "HALT",   OP_TRAP,0x025, 0,                                                               }, */
};

const struct macro_fmt MACRO_TABLE[] =
{
    { ".ASCII",     M_ASCII,    -1, MO_ASCII    },
    { ".BLKW",      M_BLKW,     1,  MO_IMM,     },
    { ".FILL",      M_FILL,     1,  MO_IMM,     },
    { ".ORIGIN",    M_ORIGIN,   1,  MO_IMM,     },

 /* { ".SEGMENT",   M_SEGMENT,  1,  MO_IMM      } */
};

int get_instr_table_size(void)
{
    return ARRAYSIZE(INSTR_TABLE);
}

int get_macro_table_size(void)
{
    return ARRAYSIZE(MACRO_TABLE);
}
