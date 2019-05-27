/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*
 * lc3tools - An implementation of the LC-3 ISA and assorted tools.           *
 * Copyright (C) 2018-2019 Wes Hampson.                                       *
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
 *   Desc: LC-3c instruction set architecture constants and definitions.
 *============================================================================*/

#ifndef __LC3_H
#define __LC3_H

#include <stdint.h>
#include <lc3tools.h>

/*
 * Number of general purpose registers.
 */
#define GPREGS          8

/*
 * Privilege modes.
 */
#define PRIV_SUPER      0
#define PRIV_USER       1

/*
 * Exception vectors.
 */
#define E_PRIV          0x00    /* Privilege Mode Violation exception */
#define E_OPCODE        0x01    /* Illegal Opcode exception */
#define E_OPADDR        0x02    /* Illegal Operand Address exception */

/*
 * Important memory addresses.
 */
#define A_TVT           0x0000  /* Trap Vector Table */
#define A_IVT           0x0200  /* Interrupt Vector Table */
#define A_SSP           0x3000  /* default supervisor stack pointer */
#define A_USP           0xFE00  /* default user stack pointer */
#define A_KBSR          0xFE00  /* keyboard status register */
#define A_KBDR          0xFE02  /* keyboard data register */
#define A_DSR           0xFE04  /* display status register */
#define A_DDR           0xFE06  /* display data register */
#define A_ICCR          0xFE10  /* interrupt controller command register */
#define A_ICDR          0xFE12  /* interrupt controller data register */
#define A_MCR           0xFFFE  /* machine control register */

/*
 * Device register fields.
 */
#define KBSR_RD         0x8000  /* keyboard ready */
#define KBSR_IE         0x4000  /* keyboard interrupt enable */
#define DSR_RD          0x8000  /* display ready */
#define DSR_IE          0x4000  /* display interrupt enable */
#define MCR_CE          0x8000  /* machine clock enable */

/*
 * LC-3 data types.
 */
typedef uint8_t         lc3byte;
typedef uint16_t        lc3word;
typedef int16_t         lc3sword;

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
            lc3word privilege   : 1;    /* priv. level; super = 0, user = 1 */
        };
        lc3word value;                  /* (aggregate value) */
    } psr;                  /* processor status register */
    int     ben;            /* branch enable flag */
    int     intf;           /* interrupt flag */
    lc3byte intv;           /* interrupt vector */
    lc3byte intp;           /* interrupt priority */
    int     state;          /* current state */
};

/*
 * LC-3c opcodes.
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

#endif /* __LC3_H */
