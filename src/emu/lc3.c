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
 *   File: src/emu/lc3.c
 * Author: Wes Hampson
 *   Desc: LC-3b instruction set architecture implementation.
 *
 *         The LC-3b is a minor extension of the LC-3 ISA not described in
 *         Patt & Patel's "Introduction to Computing Systems" (2nd Edition).
 *         However, Patt has a partial description on his university webpage:
 *             http://users.ece.utexas.edu/~patt/18f.460N/handouts/appA.pdf.
 *
 *         SUMMARY OF CHANGES IN LC-3b:
 *           - Memory is now byte-addressable.
 *           - LD, ST instructions (data storage using PC-relative addressing)
 *             have been removed.
 *           - LDI, STI instructions now use register-based addressing.
 *           - LDB, STB instructions have been added to manipulate individual
 *             bytes of memory.
 *           - LDR, STR instructions have been renamed to LDW, STW to indicate
 *             that the instructions manipulate words of memory.
 *           - SHF instruction has been added to allow for arithmetic and
 *             logical shifts.
 *           - Bitwise NOT ALU operation has been replaced with bitwise XOR. The
 *             instruction encoding for NOT remains unchanged.
 *============================================================================*/



#include <stdio.h>
#include <lc3.h>

/* ===== USEFUL MACROS ===== */

/*
 * Instruction Register fields.
 */
#define OPCODE()    ((cpu.ir & 0xF000) >> 12)
#define DR()        ((cpu.ir & 0x0E00) >> 9)
#define SR()        ((cpu.ir & 0x01C0) >> 6)
#define SR1()       ((cpu.ir & 0x01C0) >> 6)
#define SR2()       (cpu.ir & 0x0007)
#define BASER()     ((cpu.ir & 0x01C0) >> 6)
#define IMM4()      (cpu.ir & 0x000F)
#define IMM5()      (cpu.ir & 0x001F)
#define OFF6()      (cpu.ir & 0x003F)
#define OFF9()      (cpu.ir & 0x01FF)
#define OFF11()     (cpu.ir & 0x07FF)
#define TRAPVECT()  (cpu.ir & 0x00FF)
#define IR_N()      (cpu.ir & 0x0800)
#define IR_Z()      (cpu.ir & 0x0400)
#define IR_P()      (cpu.ir & 0x0200)
#define A()         (cpu.ir & 0x0020)   /* ALU operation (ADD/AND/XOR) */
#define A_JSR()     (cpu.ir & 0x0800)   /* ALU operation (JSR) */
#define D()         (cpu.ir & 0x0010)   /* direction (SHF) */

/*
 * Processor State Register fields.
 */
#define PRIORITY()  (cpu.psr.priority)
#define PRIVILEGE() (cpu.psr.privilege)
#define N()         (cpu.psr.n)
#define Z()         (cpu.psr.z)
#define P()         (cpu.psr.p)


/*
 * LC-3 instruction function pointer type.
 */
typedef void (*exec_op)(void);


/* ===== PRIVATE FUNCTION DECLARATIONS ===== */
/* See function definition for details. */

static void exec_br(void);
static void exec_add(void);
static void exec_ldb(void);
static void exec_stb(void);
static void exec_jsr(void);
static void exec_and(void);
static void exec_ldw(void);
static void exec_stw(void);
static void exec_rti(void);
static void exec_xor(void);
static void exec_ill(void);
static void exec_jmp(void);
static void exec_shf(void);
static void exec_lea(void);
static void exec_trap(void);

static inline void setcc(void);

static inline lc3word reg_r(int n);
static inline void reg_w(int n, lc3word data);

static inline lc3word mem_r(lc3word addr);
static inline lc3byte mem_rb(lc3word addr);

static inline void mem_w(lc3word addr, lc3word data);
static inline void mem_wb(lc3word addr, lc3byte data);

static inline lc3sword sign_extend(lc3word val, int pos);

/*
 * Opcode jump table.
 */
static exec_op exec_table[NUM_OPS] = {
    exec_br,    exec_add,   exec_ldb,   exec_stb,
    exec_jsr,   exec_and,   exec_ldw,   exec_stw,
    exec_rti,   exec_xor,   exec_ill,   exec_ill,
    exec_jmp,   exec_shf,   exec_lea,   exec_trap
};

/*
 * The CPU state.
 */
static lc3cpu cpu;


/* ===== PUBLIC FUNCTION DEFINITIONS ===== */

/*
 * Reset the CPU. Memory is not modified.
 */
void lc3_reset(void)
{
    int i;

    for (i = 0; i < GPREGS; i++) reg_w(i, 0);
    cpu.pc = 0; /* TODO: reset vector */
    cpu.ir = 0;
    cpu.mar = 0;
    cpu.mdr = 0;
    cpu.saved_ssp = 0;
    cpu.saved_usp = 0;
    cpu.intf = 0;
    cpu.intv = 0;
    cpu.psr.value = 0;
    setcc();
}

/*
 * Read an array of bytes from main memory.
 */
void lc3_readmem(lc3byte *data, lc3word addr, size_t nbytes)
{
    /* TODO: arg check */

    int i;

    for (i = 0; i < nbytes; i++) {
        data[i] = mem_rb(addr + i);
    }
}

/*
 * Write an array of bytes into main memory.
 */
void lc3_writemem(lc3word addr, lc3byte *data, size_t nbytes)
{
    /* TODO: arg check */

    int i;

    for (i = 0; i < nbytes; i++) {
        mem_wb(addr + i, data[i]);
    }
}

/*
 * Begin executing instructions at the specified address.
 * (TODO: when does it halt?)
 */
void lc3_execute(lc3word addr)
{
    int ncycles;

    ncycles = 50;
    while (ncycles-- > 0) {
        /* Fetch */
        cpu.ir = mem_r(cpu.pc);
        cpu.pc += 2;

        /* Decode */
        exec_table[OPCODE()]();

        /* TODO: interrupts, halt check, etc. */
    }
}

/*
 * Print all registers to STDOUT.
 */
void lc3_printregs(void)
{
    printf("R0   = 0x%04x\tR1   = 0x%04x\tR2   = 0x%04x\tR3   = 0x%04x\n",
        reg_r(0), reg_r(1), reg_r(2), reg_r(3));
    printf("R4   = 0x%04x\tR5   = 0x%04x\tR6   = 0x%04x\tR7   = 0x%04x\n",
        reg_r(4), reg_r(5), reg_r(6), reg_r(7));
    printf("PC   = 0x%04x\tIR   = 0x%04x\tMAR  = 0x%04x\tMDR  = 0x%04x\n",
        cpu.pc, cpu.ir, cpu.mar, cpu.mdr);
    printf("SSP  = 0x%04x\tUSP  = 0x%04x\tINTV = 0x%02x\tINTF = %d\n",
        cpu.saved_ssp, cpu.saved_usp, cpu.intv, cpu.intf);
    printf("PSR  = 0x%04x [ ", cpu.psr.value);
    printf("Privilege = %d, ", PRIVILEGE());
    printf("Priority = %d, ", PRIORITY());
    printf("N = %d, Z = %d, P = %d ]\n", N(), Z(), P());

    /* TODO: memory-mapped registers */
}

/* ===== PRIVATE FUNCTION DEFINITIONS ===== */

/*
 * BR: Conditional Branch
 * Opcode 0000
 */
static void exec_br(void)
{
    lc3sword pcoffset;

    pcoffset = sign_extend(OFF9(), 9);
    if ((N() && IR_N()) || (Z() && IR_Z()) || (P() && IR_P())) {
        cpu.pc += (pcoffset << 1);
    }

    printf("BR");
    if (IR_N()) printf("n");
    if (IR_Z()) printf("z");
    if (IR_P()) printf("p");
    printf("\t#%d\n", pcoffset);
}

/*
 * ADD: Arithmetic add
 * Opcode 0001
 */
void exec_add(void)
{
    lc3word op1, op2;
    lc3word result;

    op1 = reg_r(SR1());
    op2 = (A())
        ? sign_extend(IMM5(), 5)
        : reg_r(SR2());
    result = op1 + op2;
    reg_w(DR(), result);
    setcc();

    printf("ADD\tR%d, R%d, ", DR(), SR1());
    if (A())
        printf("#%d\n", sign_extend(IMM5(), 5));
    else
        printf("R%d\n", SR2());
}

/*
 * LDB: Load byte
 * Opcode 0010
 */
static void exec_ldb(void)
{
    printf("LDB\tR%d, R%d, #%d\n", DR(), BASER(), OFF6());
}

/*
 * STB: Store byte
 * Opcode 0011
 */
static void exec_stb(void)
{
    printf("STB\tR%d, R%d, #%d\n", SR(), BASER(), OFF6());
}

/*
 * JSR: Jump to sub-routine
 * Opcode 0100
 */
static void exec_jsr(void)
{
    printf("JSR");
    if (A_JSR()) {
        printf("\t#%d\n", OFF11());
    }
    else {
        printf("R\tR%d\n", BASER());
    }
}

/*
 * AND: Bitwise and
 * Opcode 0101
 */
static void exec_and(void)
{
    lc3word op1, op2;
    lc3word result;

    op1 = reg_r(SR1());
    op2 = (A())
        ? sign_extend(IMM5(), 5)
        : reg_r(SR2());
    result = op1 & op2;
    reg_w(DR(), result);
    setcc();

    printf("AND\tR%d, R%d, ", DR(), SR1());
    if (A())
        printf("#%d\n", sign_extend(IMM5(), 5));
    else
        printf("R%d\n", SR2());
}

/*
 * LDW: Load word
 * Opcode 0110
 */
static void exec_ldw(void)
{
    printf("LDW\tR%d, R%d, #%d\n", SR(), BASER(), OFF6());
}

/*
 * STW: Store word
 * Opcode 0111
 */
static void exec_stw(void)
{
    printf("STW\tR%d, R%d, #%d\n", SR(), BASER(), OFF6());
}

/*
 * RTI: Return from interrupt
 * Opcode 1000
 *
 * Raises a privilege mode exception if executed while PRIVILEGE = 1 (user).
 */
static void exec_rti(void)
{
    if (PRIVILEGE() == PRIV_USER) {
        printf("Privilege mode violation!\n");
    }
    else {
        printf("RTI\n");
    }
}

/*
 * XOR: Bitwise exclusive or
 * Opcode 1101
 */
static void exec_xor(void)
{
    lc3word op1, op2;
    lc3word result;

    op1 = reg_r(SR1());
    op2 = (A())
        ? sign_extend(IMM5(), 5)
        : reg_r(SR2());
    result = op1 ^ op2;
    reg_w(DR(), result);
    setcc();

    printf("XOR\tR%d, R%d, ", DR(), SR1());
    if (A())
        printf("#%d\n", sign_extend(IMM5(), 5));
    else
        printf("R%d\n", SR2());
}

/*
 * Illegal opcode detected. Triggers an illegal opcode exception.
 * Opcodes 1010 and 1011.
 */
static void exec_ill(void)
{
    /* TODO: raise illegal opcode exception */
    printf("Illegal opcode! (0x%X)\n", OPCODE());
}

/*
 * JMP: Unconditional branch
 * Opcode 1100
 */
static void exec_jmp(void)
{
    printf("JMP\tR%d\n", BASER());
}

/*
 * SHF: Bit shifting operations
 * Opcode 1101
 */
static void exec_shf(void)
{
    lc3word op1, op2;
    lc3word result;

    op1 = reg_r(SR1());
    op2 = IMM4();
    if (D()) {
        if (A()) {
            /* arithmetic right shift */
            result = (lc3sword) op1 >> op2;
        }
        else {
            /* logical right shift */
            result = op1 >> op2;
        }
    }
    else {
        result = op1 << op2;
    }
    reg_w(DR(), result);
    setcc();

    if (D()) {
        printf("RSHF");
        if (A())
            printf("A");
        else
            printf("L");
    }
    else
        printf("LSHF");
    printf("\tR%d, R%d, ", DR(), SR1());
    printf("#%d\n", IMM4());
}

/*
 * LEA: Load effective address
 * Opcode 1110
 */
static void exec_lea(void)
{
    printf("LEA\tR%d, #%d\n", DR(), OFF9());
}

/*
 * TRAP: Execute trap service routine
 * Opcode 1111
 */
static void exec_trap(void)
{
    printf("TRAP\t#%d\n", TRAPVECT());
}

/*
 * Update the CPU's condition codes based on the value in the destination
 * register.
 */
static inline void setcc(void)
{
    lc3word val;

    val = reg_r(DR());
    cpu.psr.n = (val & 0x8000) == 0x8000;
    cpu.psr.z = val == 0;
    cpu.psr.p = !(cpu.psr.n || cpu.psr.z);
}

/*
 * Read a word from a general-purpose register.
 */
static inline lc3word reg_r(int n)
{
    return cpu.r[n & 0x07];
}

/*
 * Write a word to a general-purpose register.
 */
static inline void reg_w(int n, lc3word data)
{
    cpu.r[n & 0x07] = data;
}

/*
 * Read a word from main memory.
 * Address is word-aligned.
 */
static inline lc3word mem_r(lc3word addr)
{
    cpu.mar = addr;
    cpu.mdr = (cpu.m[(cpu.mar & 0xFFFE) + 1] << 8) | cpu.m[cpu.mar & 0xFFFE];
    return cpu.mdr;
}

/*
 * Read a byte from a main memory.
 */
static inline lc3byte mem_rb(lc3word addr)
{
    return (addr & 0x0001)
        ? (mem_r(addr) >> 8) & 0xFF
        : mem_r(addr) & 0xFF;
}

/*
 * Write a word to main memory.
 * Address is word-aligned.
 */
static inline void mem_w(lc3word addr, lc3word data)
{
    cpu.mar = addr;
    cpu.mdr = data;
    cpu.m[(cpu.mar & 0xFFFE) + 1] = (cpu.mdr >> 8) & 0xFF;
    cpu.m[cpu.mar & 0xFFFE] = cpu.mdr & 0xFF;
}

/*
 * Write a byte to main memory.
 */
static inline void mem_wb(lc3word addr, lc3byte data)
{
    cpu.mar = addr;
    cpu.mdr = (cpu.mar & 0x0001) ? data << 8 : data;
    cpu.m[cpu.mar] = data;
}

/*
 * Sign-extend a value from the given bit position.
 * This effectively converts an unsigned word into a signed word.
 */
static inline lc3sword sign_extend(lc3word val, int pos)
{
    lc3word mask;

    mask = 1 << (pos - 1);
    return (lc3sword) ((val ^ mask) - mask);
}
