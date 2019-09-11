#ifndef __TYPES_H
#define __TYPES_H

#ifdef __cplusplus
extern "C"
{
#endif

#include <stdint.h>

/*
 * LC-3 data types.
 */
typedef uint8_t         lc3byte;
typedef uint16_t        lc3word;
typedef int16_t         lc3sword;

/*
 * LC-3c opcodes.
 */
enum lc3op
{
    OP_BR,      /* conditional branch (pc-relative) */
    OP_ADD,     /* ADD */
    OP_LDB,     /* load byte */
    OP_STB,     /* store byte */
    OP_JSR,     /* jump to sub-routine */
    OP_AND,     /* bitwise AND */
    OP_LDW,     /* load word */
    OP_STW,     /* store word */
    OP_RTI,     /* return from interrupt (requires supervisor) */
    OP_XOR,     /* bitwise XOR */
    OP_LDI,     /* load word, indirect addresing */
    OP_STI,     /* store word, indirect addressing */
    OP_JMP,     /* unconditional branch (register-absolute) */
    OP_SHF,     /* bitwise shift */
    OP_LEA,     /* load effective address */
    OP_TRAP,    /* execute trap service routine */
    NUM_OPS     /* (number of opcodes) */
};

#ifdef __cplusplus
}
#endif

#endif  /* __TYPES_H */
