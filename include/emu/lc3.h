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
#define MEMSIZE         (1 << 16)

/*
 * Number of general purpose registers.
 */
#define GPREGS          8

/*
 * Memory-mapped I/O addresses.
 */
#define A_KBSR          0xFE00  /* keyboard status register */
#define A_KBDR          0xFE02  /* keyboard data register */
#define A_DSR           0xFE04  /* display status register */
#define A_DDR           0xFE06  /* display data register */
#define A_MCR           0xFFFE  /* machine control register */

/*
 * Machine Control Register fields.
 */
#define CLOCK_ENABLE    0x8000

/*
 * Privilege modes.
 */
#define PRIV_SUPER      0
#define PRIV_USER       1

/*
 * LC-3 data types.
 */
typedef unsigned char   lc3byte;
typedef unsigned short  lc3word;
typedef short           lc3sword;

/*
 * The LC-3 CPU state.
 */
struct lc3cpu {
    lc3word r[GPREGS];      /* general purpose registers */
    lc3word pc;             /* program counter */
    lc3word ir;             /* instruction register */
    lc3word mar;            /* memory address register */
    lc3word mdr;            /* memory data register */
    lc3word saved_ssp;      /* saved supervisor stack pointer */
    lc3word saved_usp;      /* saved user stack pointer */
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
    int     intf;           /* interrupt flag */
    lc3byte intv;           /* interrupt vector */
    lc3byte m[MEMSIZE];     /* random access memory */
};

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
    OP_LDI,     /* load word, indirect addresing */
    OP_STI,     /* store word, indirect addressing */
    OP_JMP,     /* unconditional branch (register-absolute) */
    OP_SHF,     /* bitwise shift */
    OP_LEA,     /* load effective address */
    OP_TRAP,    /* execute trap service routine */
    NUM_OPS     /* (number of opcodes) */
};

/*
 * LC-3 registers.
 */
enum lc3reg {
    R_0,
    R_1,
    R_2,
    R_3,
    R_4,
    R_5,
    R_6,
    R_7,
    R_PC,
    R_IR,
    R_MAR,
    R_MDR,
    R_SSP,
    R_USP,
    R_PSR,
    R_KBSR,
    R_KBDR,
    R_DSR,
    R_DDR,
    R_MCR,
    NUM_REGS
};

/* ===== FUNCTIONS DEFINED IN src/emu/lc3.c ===== */

/*
 * Reset all registers to zero.
 * Memory outside the memory-mapped I/O region is not modified.
 */
void lc3_reset(void);

/*
 * Read a register value.
 *
 * @param reg   the register number (see lc3reg enum)
 * @param data  the address where the register value will be written to
 * @return      -1 for a bad pointer or invalid register number;
 *               0 for a successful read
 */
int lc3_readreg(int reg, lc3word *data);

/*
 * Set a register value.
 *
 * @param reg   the register number (see lc3reg enum)
 * @param data  the value to store in the register
 * @return      -1 for an invalid register number;
 *               0 for a successful write
 */
int lc3_writereg(int reg, lc3word data);

/*
 * Read an array of bytes from main memory.
 *
 * @param data  a pre-allocated buffer of at least n bytes in size used for
 *              storing the read data
 * @param addr  the address to start reading from
 * @param n     the number of bytes to read
 * @return      -1 for a bad argument;
 *               0 for a successful read
 */
int lc3_readmem(lc3byte *data, lc3word addr, size_t n);

/*
 * Write an array of bytes into main memory.
 *
 * @param addr  the address to start writing to
 * @param data  a buffer of at most n bytes containing the data to be written
 * @param n     the number of bytes to write
 * @return      -1 for a bad argument;
 *               0 for a successful write
 */
int lc3_writemem(lc3word addr, lc3byte *data, size_t nbytes);

/*
 * Execute exactly one instruction cycle.
 * The ability to step is not dependent on the state of the clock enable bit
 * in MCR.
 */
void lc3_step(void);

/*
 * Execute instructions for as long as the clock enable bit in MCR is high.
 */
void lc3_run(void);

/*
 * Print all registers to STDOUT.
 */
void lc3_printregs(void);

#endif /* __LC3_H */
