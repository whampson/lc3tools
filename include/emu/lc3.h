/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*
 * lc3tools - An implementation of the LC-3 ISA and assorted tools.           *
 * Copyright (C) 2018 Wes Hampson.                                            *
 *                                                                            *
 * This program is free software; you can redistribute it and/or modify       *
 * it under the terms of the GNU General Public License as published by       *
 * the Free Software Foundation; either version 2 of the License, or          *
 * (at your option) any later version.                                        *
 *                                                                            *
 * This program is distributed in the hope that it will be useful,            *
 * but WITHOUT ANY WARRANTY; without even the implied warranty of             *
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the              *
 * GNU General Public License for more details                                *
 *~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

/*==============================================================================
 *   File: include/emu/lc3.h
 * Author: Wes Hampson
 *   Desc: LC-3b instruction set architecture constants and definitions.
 *============================================================================*/

#ifndef __LC3_H
#define __LC3_H

/*
 * LC-3 address space size.
 */
#define MEMSIZE (1 << 16)

/*
 * Number of general purpose registers.
 */
#define GPREGS  8

/*
 * Memory-mapped I/O addresses.
 */
#define KBSR    0xFE00  /* keyboard status register */
#define KBDR    0xFE02  /* keyboard data register */
#define DSR     0xFE04  /* display status register */
#define DDR     0xFE06  /* display data register */
#define MCR     0xFFFE  /* machine control register */

/*
 * Privilege modes.
 */
#define PRIV_SUPER  0
#define PRIV_USER   1

/*
 * LC-3 data types.
 */
typedef unsigned char   lc3byte;
typedef unsigned short  lc3word;
typedef short           lc3sword;

/*
 * The LC-3 CPU state.
 */
typedef struct {
    lc3word r[GPREGS];      /* general purpose registers */
    lc3word pc;             /* program counter */
    lc3word ir;             /* instruction register */
    lc3word mar;            /* memory address register */
    lc3word mdr;            /* memory data register */
    lc3word saved_ssp;      /* saved supervisor stack pointer */
    lc3word saved_usp;      /* saved user stack pointer */
    int     intf;           /* interrupt flag */
    lc3byte intv;           /* interrupt vector */
    union {
        struct {
            lc3word p           : 1;    /* positive flag */
            lc3word z           : 1;    /* zero flag */
            lc3word n           : 1;    /* negative flag */
            lc3word _reserved1  : 5;    /* (reserved, do not use) */
            lc3word priority    : 3;    /* priority level */
            lc3word _reserved0  : 4;    /* (reserved, do not use) */
            lc3word privilege   : 1;    /* privilege level; super = 0, user = 1 */
        };
        lc3word value;                  /* aggregate value */
    } psr;                  /* processor status register */
    lc3byte m[MEMSIZE];     /* random access memory */
} lc3cpu;

/*
 * LC-3b opcodes.
 */
enum lc3op {
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
    OP_UNUSED1, /* (unused, causes invalid opcode exception) */
    OP_UNUSED2, /* (unused, causes invalid opcode exception) */
    OP_JMP,     /* unconditional branch (register-absolute) */
    OP_SHF,     /* bitwise shift */
    OP_LEA,     /* load effective address */
    OP_TRAP,    /* execute trap service routine */
    NUM_OPS     /* (number of opcodes) */
};

/* ===== FUNCTIONS DEFINED IN src/emu/lc3.c ===== */

/*
 * Reset the CPU. Memory is not modified.
 */
void lc3_reset(void);

/*
 * Read an array of bytes from main memory.
 */
void lc3_readmem(lc3byte *data, lc3word addr, size_t nbytes);

/*
 * Write an array of bytes into main memory.
 */
void lc3_writemem(lc3word addr, lc3byte *data, size_t nbytes);

/*
 * Begin executing instructions at the specified address.
 * (TODO: when does it halt?)
 */
void lc3_execute(lc3word addr);

/*
 * Print all registers to STDOUT.
 */
void lc3_printregs(void);

#endif /* __LC3_H */
