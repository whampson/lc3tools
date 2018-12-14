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
 *           - LDB, STB instructions have been added to manipulate individual
 *             bytes of memory.
 *           - LDR, STR instructions have been renamed to LDW, STW to indicate
 *             that the instructions manipulate words of memory.
 *           - SHF instruction has been added to allow for arithmetic and
 *             logical shifts.
 *           - `XOR` instruction has been added and bitwise NOT ALU operation
 *             has been replaced with bitwise XOR. The instruction encoding for
 *             `NOT` remains unchanged.
 *============================================================================*/


/*
 * TODO:
 *   - Implement microcode
 *   - TEST, TEST, TEST!
 */


#include <stdio.h>
#include <lc3tools.h>
#include <lc3.h>
#include <mem.h>

/* ===== USEFUL MACROS ===== */

/*
 * Instruction Register fields.
 */
#define OPCODE()        ((cpu.ir & 0xF000) >> 12)
#define DR()            ((cpu.ir & 0x0E00) >> 9)
#define SR()            ((cpu.ir & 0x0E00) >> 9)
#define SR1()           ((cpu.ir & 0x01C0) >> 6)
#define SR2()           (cpu.ir & 0x0007)
#define BASER()         ((cpu.ir & 0x01C0) >> 6)
#define IMM4()          (cpu.ir & 0x000F)
#define IMM5()          (cpu.ir & 0x001F)
#define OFF6()          (cpu.ir & 0x003F)
#define OFF9()          (cpu.ir & 0x01FF)
#define OFF11()         (cpu.ir & 0x07FF)
#define TRAPVECT()      (cpu.ir & 0x00FF)
#define IR_N()          (cpu.ir & 0x0800)
#define IR_Z()          (cpu.ir & 0x0400)
#define IR_P()          (cpu.ir & 0x0200)
#define A()             (cpu.ir & 0x0020)   /* ALU operation (ADD/AND/XOR) */
#define A_JSR()         (cpu.ir & 0x0800)   /* ALU operation (JSR) */
#define D()             (cpu.ir & 0x0010)   /* direction (SHF) */

/*
 * Processor State Register fields.
 */
#define PRIORITY()      (cpu.psr.priority)
#define PRIVILEGE()     (cpu.psr.privilege)
#define N()             (cpu.psr.n)
#define Z()             (cpu.psr.z)
#define P()             (cpu.psr.p)

#define SET_PRIORITY(x) (cpu.psr.priority = x)
#define SET_PRIVILEGE(x)(cpu.psr.privilege = x)
#define SET_N(x)        (cpu.psr.n = x)
#define SET_Z(x)        (cpu.psr.z = x)
#define SET_P(x)        (cpu.psr.p = x)

/*
 * Memory-mapped I/O registers.
 */
#define KBSR()          (mem_r(A_KBSR))
#define KBDR()          (mem_r(A_KBDR))
#define DSR()           (mem_r(A_DSR))
#define DDR()           (mem_r(A_DDR))
#define MCR()           (mem_r(A_MCR))

#define SET_KBSR(x)     (mem_w(A_KBSR, x))
#define SET_KBDR(x)     (mem_w(A_KBDR, x))
#define SET_DSR(x)      (mem_w(A_DSR,  x))
#define SET_DDR(x)      (mem_w(A_DDR,  x))
#define SET_MCR(x)      (mem_w(A_MCR,  x))


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
static void exec_ldi(void);
static void exec_sti(void);
static void exec_jmp(void);
static void exec_shf(void);
static void exec_lea(void);
static void exec_trap(void);

static void interrupt(void);
static void exception(int vec);

static inline void setcc(void);

static inline lc3word reg_r(int n);
static inline void reg_w(int n, lc3word data);

static inline lc3word mem_r(lc3word addr);
static inline lc3byte mem_rb(lc3word addr);

static inline void mem_w(lc3word addr, lc3word data);
static inline void mem_wb(lc3word addr, lc3byte data);

static inline void stack_push(lc3word data);
static inline lc3word stack_pop(void);
static inline void stack_switch(int to_privilege);

static inline lc3sword sign_extend(lc3word val, int pos);

/*
 * Opcode jump table.
 */
static exec_op exec_table[NUM_OPS] = {
    exec_br,    exec_add,   exec_ldb,   exec_stb,
    exec_jsr,   exec_and,   exec_ldw,   exec_stw,
    exec_rti,   exec_xor,   exec_ldi,   exec_sti,
    exec_jmp,   exec_shf,   exec_lea,   exec_trap
};

/*
 * The CPU state.
 */
static struct lc3cpu cpu;


/* ===== PUBLIC FUNCTION DEFINITIONS ===== */

/*
 * Reset all registers to zero.
 * Memory outside the memory-mapped I/O region is not modified.
 */
void lc3_zero(void)
{
    int i;

    for (i = 0; i < GPREGS; i++) reg_w(i, 0);
    cpu.pc = 0;
    cpu.ir = 0;
    cpu.mar = 0;
    cpu.mdr = 0;
    cpu.saved_ssp = 0;
    cpu.saved_usp = 0;
    cpu.intf = 0;
    cpu.intv = 0;
    cpu.psr.value = 0;
    SET_KBSR(0);
    SET_KBDR(0);
    SET_DSR(0);
    SET_DDR(0);
    SET_MCR(0);
    setcc();
}

/*
 * Read a register value.
 */
int lc3_readreg(int reg, lc3word *data)
{
    if (data == NULL || reg < 0 || reg >= NUM_REGS) {
        return -1;
    }

    if (reg >= R_0 && reg <= R_7) {
        *data = reg_r(reg);
        return 0;
    }

    switch (reg) {
        case R_PC:      *data = cpu.pc;         break;
        case R_IR:      *data = cpu.ir;         break;
        case R_MAR:     *data = cpu.mar;        break;
        case R_MDR:     *data = cpu.mdr;        break;
        case R_SSP:     *data = cpu.saved_ssp;  break;
        case R_USP:     *data = cpu.saved_usp;  break;
        case R_PSR:     *data = cpu.psr.value;  break;
        case R_KBSR:    *data = KBSR();         break;
        case R_KBDR:    *data = KBDR();         break;
        case R_DSR:     *data = DSR();          break;
        case R_DDR:     *data = DDR();          break;
        case R_MCR:     *data = MCR();          break;
    }

    return 0;
}

/*
 * Set a register value.
 */
int lc3_writereg(int reg, lc3word data)
{
    if (reg < 0 || reg >= NUM_REGS) {
        return -1;
    }

    if (reg >= R_0 && reg <= R_7) {
        reg_w(reg, data);
        return 0;
    }

    switch (reg) {
        case R_PC:      cpu.pc = data;          break;
        case R_IR:      cpu.ir = data;          break;
        case R_MAR:     cpu.mar = data;         break;
        case R_MDR:     cpu.mdr = data;         break;
        case R_SSP:     cpu.saved_ssp = data;   break;
        case R_USP:     cpu.saved_usp = data;   break;
        case R_PSR:     cpu.psr.value = data;   break;
        case R_KBSR:    SET_KBSR(data);         break;
        case R_KBDR:    SET_KBDR(data);         break;
        case R_DSR:     SET_DSR(data);          break;
        case R_DDR:     SET_DDR(data);          break;
        case R_MCR:     SET_MCR(data);          break;
    }

    return 0;
}

/*
 * Read an array of bytes from main memory.
 */
int lc3_readmem(lc3byte *data, lc3word addr, size_t n)
{
    int i;

    if (data == NULL || n > MEM_SIZE) {
        return -1;
    }

    for (i = 0; i < n; i++) {
        data[i] = mem_rb(addr + i);
    }

    return 0;
}

/*
 * Write an array of bytes into main memory.
 */
int lc3_writemem(lc3word addr, lc3byte *data, size_t n)
{
    int i;

    if (data == NULL || n > MEM_SIZE) {
        return -1;
    }

    for (i = 0; i < n; i++) {
        mem_wb(addr + i, data[i]);
    }

    return 0;
}

/*
 * Execute exactly one instruction cycle.
 * The ability to step is not dependent on the state of the clock enable bit
 * in MCR.
 */
void lc3_step(void)
{
    #ifdef DEBUG
    printf("%04x: ", cpu.pc);
    #endif

    cpu.ir = mem_r(cpu.pc);
    cpu.pc += 2;

    if (cpu.intf) {
        interrupt();
        return;
    }

    exec_table[OPCODE()]();
}

/*
 * Execute instructions for as long as the clock enable bit in MCR is high.
 */
void lc3_run(void)
{
    while ((MCR() & MCR_CE) == MCR_CE) {
        lc3_step();
    }
}

/*
 * Print all registers to STDOUT.
 */
void lc3_printregs(void)
{
    printf("R0   = 0x%04x\tR1   = 0x%04x\tR2   = 0x%04x\tR3   = 0x%04x\n",
        reg_r(R_0), reg_r(R_1), reg_r(R_2), reg_r(R_3));
    printf("R4   = 0x%04x\tR5   = 0x%04x\tR6   = 0x%04x\tR7   = 0x%04x\n",
        reg_r(R_4), reg_r(R_5), reg_r(R_6), reg_r(R_7));
    printf("PC   = 0x%04x\tIR   = 0x%04x\tMAR  = 0x%04x\tMDR  = 0x%04x\n",
        cpu.pc, cpu.ir, cpu.mar, cpu.mdr);
    printf("SSP  = 0x%04x\tUSP  = 0x%04x\tINTV = 0x%02x\tINTF = %d\n",
        cpu.saved_ssp, cpu.saved_usp, cpu.intv, cpu.intf);
    printf("PSR  = 0x%04x [ ", cpu.psr.value);
    printf("Privilege = %d, ", PRIVILEGE());
    printf("Priority = %d, ", PRIORITY());
    printf("N = %d, Z = %d, P = %d ]\n", N(), Z(), P());
    printf("KBSR = 0x%04x\tKBDR = 0x%04x\tDSR  = 0x%04x\rDDR  = 0x%04x\n",
        KBSR(), KBDR(), DSR(), DDR());
    printf("MCR  = 0x%04x\n", MCR());
}


/* ===== CPU INSTRUCTIONS ===== */

/*
 * BR: Conditional Branch
 * Opcode 0000
 */
static void exec_br(void)
{
    #ifdef DEBUG
    printf("BR");
    if (IR_N()) printf("n");
    if (IR_Z()) printf("z");
    if (IR_P()) printf("p");
    printf("\t#%d\n", (sign_extend(OFF9(), 9) << 1));
    #endif

    if ((N() && IR_N()) || (Z() && IR_Z()) || (P() && IR_P())) {
        cpu.pc += (sign_extend(OFF9(), 9) << 1);
    }
}

/*
 * ADD: Arithmetic add
 * Opcode 0001
 */
void exec_add(void)
{
    lc3word op1, op2;
    lc3word result;

    #ifdef DEBUG
    printf("ADD\tR%d, R%d, ", DR(), SR1());
    if (A())
        printf("#%d\n", sign_extend(IMM5(), 5));
    else
        printf("R%d\n", SR2());
    #endif

    op1 = reg_r(SR1());
    op2 = (A())
        ? sign_extend(IMM5(), 5)
        : reg_r(SR2());
    result = op1 + op2;
    reg_w(DR(), result);
    setcc();
}

/*
 * LDB: Load byte
 * Opcode 0010
 */
static void exec_ldb(void)
{
    lc3word addr;
    lc3word data;

    #ifdef DEBUG
    printf("LDB\tR%d, R%d, #%d\n", DR(), BASER(), sign_extend(OFF6(), 6));
    #endif

    addr = reg_r(BASER()) + sign_extend(OFF6(), 6);
    data = sign_extend(mem_rb(addr), 8);
    reg_w(DR(), data);
    setcc();
}

/*
 * STB: Store byte
 * Opcode 0011
 */
static void exec_stb(void)
{
    lc3word addr;
    lc3byte data;

    #ifdef DEBUG
    printf("STB\tR%d, R%d, #%d\n", SR(), BASER(), sign_extend(OFF6(), 6));
    #endif

    addr = reg_r(BASER()) + sign_extend(OFF6(), 6);
    data = reg_r(SR()) & 0xFF;
    mem_wb(DR(), data);
}

/*
 * JSR: Jump to sub-routine
 * Opcode 0100
 *
 * If the target address is retrieved from a register and the address is odd,
 * an Illegal Operand Address exception is raised.
 */
static void exec_jsr(void)
{
    lc3word addr;

    #ifdef DEBUG
    printf("JSR");
    if (A_JSR()) {
        printf("\t#%d\n", sign_extend(OFF11(), 11) << 1);
    }
    else {
        printf("R\tR%d\n", BASER());
    }
    #endif

    if (A_JSR()) {
        addr = cpu.pc + (sign_extend(OFF11(), 11) << 1);
    }
    else {
        addr = reg_r(BASER());
    }

    if (addr & 0x0001) {
        exception(E_OPADDR);
        return;
    }

    reg_w(R_7, cpu.pc);
    cpu.pc = addr;
}

/*
 * AND: Bitwise AND
 * Opcode 0101
 */
static void exec_and(void)
{
    lc3word op1, op2;
    lc3word result;

    #ifdef DEBUG
    printf("AND\tR%d, R%d, ", DR(), SR1());
    if (A())
        printf("#%d\n", sign_extend(IMM5(), 5));
    else
        printf("R%d\n", SR2());
    #endif

    op1 = reg_r(SR1());
    op2 = (A())
        ? sign_extend(IMM5(), 5)
        : reg_r(SR2());
    result = op1 & op2;
    reg_w(DR(), result);
    setcc();
}

/*
 * LDW: Load word
 * Opcode 0110
 *
 * If the word address is odd, an Illegal Operand Address exception is raised.
 */
static void exec_ldw(void)
{
    lc3word addr;
    lc3word data;

    #ifdef DEBUG
    printf("LDW\tR%d, R%d, #%d\n",
        DR(), BASER(), (sign_extend(OFF6(), 6) << 1));
    #endif

    addr = reg_r(BASER()) + (sign_extend(OFF6(), 6) << 1);
    if (addr & 0x0001) {
        exception(E_OPADDR);
        return;
    }

    data = mem_r(addr);
    reg_w(DR(), data);
    setcc();
}

/*
 * STW: Store word
 * Opcode 0111
 *
 * If the word address is odd, an Illegal Operand Address exception is raised.
 */
static void exec_stw(void)
{
    lc3word addr;
    lc3word data;

    #ifdef DEBUG
    printf("STW\tR%d, R%d, #%d\n",
        SR(), BASER(), (sign_extend(OFF6(), 6) << 1));
    #endif

    addr = reg_r(BASER()) + (sign_extend(OFF6(), 6) << 1);
    if (addr & 0x0001) {
        exception(E_OPADDR);
        return;
    }

    data = reg_r(SR());
    mem_w(addr, data);
}

/*
 * RTI: Return from interrupt
 * Opcode 1000
 *
 * If executed while PRIVILEGE = 1 (user), a Privilege Mode Violation exception
 * is raised.
 */
static void exec_rti(void)
{
    #ifdef DEBUG
    printf("RTI\n");
    #endif

    /* Invoke Privilege Mode Violation if executed from user privilege */
    if (PRIVILEGE() == PRIV_USER) {
        exception(E_PRIV);
        return;
    }

    /* Restore PC and PSR */
    cpu.pc = stack_pop();
    cpu.psr.value = stack_pop();

    /* Switch stacks if necessary */
    if (PRIVILEGE() == PRIV_USER) {
        stack_switch(PRIV_USER);
    }
}

/*
 * XOR: Bitwise exclusive OR
 * Opcode 1101
 */
static void exec_xor(void)
{
    lc3word op1, op2;
    lc3word result;

    #ifdef DEBUG
    printf("XOR\tR%d, R%d, ", DR(), SR1());
    if (A())
        printf("#%d\n", sign_extend(IMM5(), 5));
    else
        printf("R%d\n", SR2());
    #endif

    op1 = reg_r(SR1());
    op2 = (A())
        ? sign_extend(IMM5(), 5)
        : reg_r(SR2());
    result = op1 ^ op2;
    reg_w(DR(), result);
    setcc();
}

/*
 * LDI: Load word, indirect addressing
 * Opcode 1010
 *
 * If either word address is odd, an Illegal Operand Address exception is
 * raised.
 */
static void exec_ldi(void)
{
    lc3word addr;
    lc3word data;
    int n;

    #ifdef DEBUG
    printf("LDI\tR%d, R%d, #%d\n",
        DR(), BASER(), (sign_extend(OFF6(), 6) << 1));
    #endif

    n = 0;
    addr = reg_r(BASER()) + (sign_extend(OFF6(), 6) << 1);

readmem:
    if (addr & 0x0001) {
        exception(E_OPADDR);
        return;
    }

    data = mem_r(addr);
    if (n == 0) {
        n++;
        addr = data;
        goto readmem;
    }

    reg_w(DR(), data);
    setcc();
}

/*
 * STI: Store word, indirect addressing
 * Opcode 1011
 *
 * If either word address is odd, an Illegal Operand Address exception is
 * raised.
 */
static void exec_sti(void)
{
    lc3word addr;
    lc3word data;
    int n;

    #ifdef DEBUG
    printf("STI\tR%d, R%d, #%d\n",
        SR(), BASER(), (sign_extend(OFF6(), 6) << 1));
    #endif

    n = 0;
    addr = reg_r(BASER()) + (sign_extend(OFF6(), 6) << 1);

readmem:
    if (addr & 0x0001) {
        exception(E_OPADDR);
        return;
    }
    if (n == 0) {
        n++;
        addr = mem_r(addr);
        goto readmem;
    }

    data = reg_r(SR());
    mem_w(addr, data);
}

/*
 * JMP: Unconditional branch
 * Opcode 1100
 *
 * If the target address is odd, an Illegal Operand Address exception is raised.
 */
static void exec_jmp(void)
{
    lc3word addr;

    #ifdef DEBUG
    printf("JMP\tR%d\n", BASER());
    #endif

    addr = reg_r(BASER());
    if (addr & 0x0001) {
        exception(E_OPADDR);
        return;
    }

    cpu.pc = addr;
}

/*
 * SHF: Bit shifting operations
 * Opcode 1101
 */
static void exec_shf(void)
{
    lc3word op1, op2;
    lc3word result;

    #ifdef DEBUG
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
    #endif

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
}

/*
 * LEA: Load effective address
 * Opcode 1110
 */
static void exec_lea(void)
{
    lc3word addr;

    #ifdef DEBUG
    printf("LEA\tR%d, #%d\n", DR(), sign_extend(OFF9(), 9));
    #endif

    addr = cpu.pc + (sign_extend(OFF9(), 9) << 1);
    reg_w(DR(), addr);
    setcc();
}

/*
 * TRAP: Execute trap service routine
 * Opcode 1111
 */
static void exec_trap(void)
{
    #ifdef DEBUG
    printf("TRAP\t#%d\n", TRAPVECT());
    #endif

    reg_w(R_7, cpu.pc);
    cpu.pc = mem_r(TRAPVECT() << 1);
}

static void interrupt(void)
{
    lc3word vec;
    lc3byte pri;
    lc3word psr;
    lc3word pc;

    /* Clear the interrupt flag */
    cpu.intf = 0;

    /* Get interrupt vector and priority */
    vec = cpu.intv;
    pri = cpu.intp & 0x07;

    #ifdef DEBUG
    printf("!! INTERRUPT !!\n");
    printf("Vector = 0x%02x, Priority = %d\n", vec, pri);
    #endif

    /* Only service interrupt if priority is higher than current process.
       Higher priority is indicated by a lower number (0 = high, 7 = low) */
    if (pri >= PRIORITY()) {
        return;
    }

    /* Load supervisor stack if necessary */
    if (PRIVILEGE() == PRIV_USER) {
        stack_switch(PRIV_SUPER);
    }

    /* Save PSR and PC */
    stack_push(cpu.psr.value);
    stack_push(cpu.pc - 2);

    /* Switch to supervisor mode */
    SET_PRIVILEGE(0);

    /* Invoke interrupt service routine */
    cpu.pc = mem_r(A_IVT | (vec << 1));
}

static void exception(int vec)
{
    cpu.intv = vec;
    cpu.intp = PL_EXCEPTION;
    interrupt();
}


/* ===== HELPER FUNCTIONS ===== */

/*
 * Update the CPU's condition codes based on the value in the destination
 * register.
 */
static inline void setcc(void)
{
    lc3word val;

    val = reg_r(DR());
    SET_N((val & 0x8000) == 0x8000);
    SET_Z(val == 0);
    SET_P(!(cpu.psr.n || cpu.psr.z));
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
    // cpu.mdr = (cpu.m[(cpu.mar & 0xFFFE) + 1] << 8) | cpu.m[cpu.mar & 0xFFFE];

    // #ifdef DEBUG
    // printf("> Read 0x%04x from M[0x%04x]\n", cpu.mdr, cpu.mar);
    // #endif

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

    // cpu.m[(cpu.mar & 0xFFFE) + 1] = (cpu.mdr >> 8) & 0xFF;
    // cpu.m[cpu.mar & 0xFFFE] = cpu.mdr & 0xFF;

    // #ifdef DEBUG
    // printf("> Wrote 0x%04x to M[0x%04x]\n", cpu.mdr, cpu.mar);
    // #endif
}

/*
 * Write a byte to main memory.
 */
static inline void mem_wb(lc3word addr, lc3byte data)
{
    cpu.mar = addr;
    cpu.mdr = (cpu.mar & 0x0001) ? data << 8 : data;
    // cpu.m[cpu.mar] = data;
}

/*
 * Push a value onto the stack.
 * The stack pointer is stored in R6.
 */
static inline void stack_push(lc3word data)
{
    lc3word sp;

    sp = reg_r(R_6);
    sp -= 2;
    mem_w(sp, data);
}

/*
 * Pop a value from the stack.
 * The stack pointer is stored in R6.
 */
static inline lc3word stack_pop(void)
{
    lc3word sp;
    lc3word data;

    sp = reg_r(R_6);
    data = mem_r(sp);
    sp += 2;

    return data;
}

/*
 * Switch from the user-mode stack to the supervisor-mode stack and vice-versa.
 */
static inline void stack_switch(int to_privilege)
{
    lc3word sp;
    sp = reg_r(R_6);

    if (to_privilege == PRIV_SUPER) {
        cpu.saved_usp = sp;
        sp = cpu.saved_ssp;
    }
    else {
        cpu.saved_ssp = sp;
        sp = cpu.saved_usp;
    }

    reg_w(R_6, sp);
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
