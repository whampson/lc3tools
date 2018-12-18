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
 *   File: src/emu/main.c
 * Author: Wes Hampson
 *   Desc: Entry point for lc3emu.
 *============================================================================*/

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <termios.h>
#include <unistd.h>

#include <lc3tools.h>
#include <lc3.h>
#include <cpu.h>
#include <mem.h>
#include <pic.h>
#include <keyboard.h>

#define _NOP                0
#define _AND(dr,sr1,sr2)    (OP_AND<<12|dr<<9|sr1<<6|sr2&7)
#define _ANDi(dr,sr1,imm)   (OP_AND<<12|dr<<9|sr1<<6|0x20|imm&0x1F)
#define _ADD(dr,sr1,sr2)    (OP_ADD<<12|dr<<9|sr1<<6|sr2&7)
#define _ADDi(dr,sr1,imm)   (OP_ADD<<12|dr<<9|sr1<<6|0x20|imm&0x1F)
#define _NOT(dr,sr)         (OP_XOR<<12|dr<<9|sr<<6|0x3F)
#define _XOR(dr,sr1,sr2)    (OP_XOR<<12|dr<<9|sr1<<6|sr2&7)
#define _XORi(dr,sr1,imm)   (OP_XOR<<12|dr<<9|sr1<<6|0x20|imm&0x1F)
#define _LSHF(dr,sr,imm)    (OP_SHF<<12|dr<<9|sr<<6|imm&0xF)
#define _RSHFL(dr,sr,imm)   (OP_SHF<<12|dr<<9|sr<<6|0x10|imm&0xF)
#define _RSHFA(dr,sr,imm)   (OP_SHF<<12|dr<<9|sr<<6|0x30|imm&0xF)
#define _BRn(pcoff)         (OP_BR<<12|0x800|pcoff&0x1FF)
#define _BRz(pcoff)         (OP_BR<<12|0x400|pcoff&0x1FF)
#define _BRp(pcoff)         (OP_BR<<12|0x200|pcoff&0x1FF)
#define _BRnz(pcoff)        (OP_BR<<12|0xC00|pcoff&0x1FF)
#define _BRnp(pcoff)        (OP_BR<<12|0xA00|pcoff&0x1FF)
#define _BRzp(pcoff)        (OP_BR<<12|0x600|pcoff&0x1FF)
#define _BRnzp(pcoff)       (OP_BR<<12|0xE00|pcoff&0x1FF)
#define _TRAP(vec)          (OP_TRAP<<12|vec)
#define _JSR(off)           (OP_JSR<<12|0x800|off)
#define _JSRR(br)           (OP_JSR<<12|br<<6)
#define _JMP(br)            (OP_JMP<<12|br<<6)
#define _RET()              (OP_JMP<<12|0x1C0)
#define _RTI()              (OP_RTI<<12)
#define _LEA(dr,pcoff)      (OP_LEA<<12|dr<<9|pcoff&0x1FF)
#define _LDB(dr,br,off)     (OP_LDB<<12|dr<<9|br<<6|off&0x3F)
#define _LDW(dr,br,off)     (OP_LDW<<12|dr<<9|br<<6|off&0x3F)
#define _LDI(dr,br,off)     (OP_LDI<<12|dr<<9|br<<6|off&0x3F)
#define _STB(sr,br,off)     (OP_STB<<12|sr<<9|br<<6|off&0x3F)
#define _STW(sr,br,off)     (OP_STW<<12|sr<<9|br<<6|off&0x3F)
#define _STI(sr,br,off)     (OP_STI<<12|sr<<9|br<<6|off&0x3F)

#define R0  (R_0)
#define R1  (R_1)
#define R2  (R_2)
#define R3  (R_3)
#define R4  (R_4)
#define R5  (R_5)
#define R6  (R_6)
#define R7  (R_7)

/**
 * POSSIBLE OPTIONS
 * Usage: lc3emu [options] executable
 *   --memory <KiB>   available memory (default 64 KiB, max 64 KiB)
 *   --help
 *   --version
 */

static struct termios orig_termios;

static void write_word(lc3word addr, lc3word data);
static void fill_mem(lc3word addr, const lc3word *data, int n);

static void usage(const char *prog_name);

static void set_nonblock(void);
static void reset_terminal(void);

#define OS_ADDR         0x0000
#define KBD_ISR         0x0400

const lc3word os_code[] =
{
    _BRnzp(-1)
};

const lc3word isr_code[] =
{
    _LEA(R0, 7),
    _LEA(R1, 7),
    _LDI(R2, R0, 0),
    _LDW(R3, R1, 0),
    _AND(R2, R2, R3),
    _STI(R2, R0, 0),
    _RTI(),
    _NOP,
    A_KBSR,
    0x7FFF
};

void print_op(lc3word op)
{
    printf("0x%04x\r\n", op);
}

int main(int argc, char *argv[])
{
    set_nonblock();

    kbd_reset();
    mem_reset();
    pic_reset();
    cpu_reset();

    fill_mem(OS_ADDR, os_code, sizeof(os_code) / sizeof(lc3word));
    fill_mem(KBD_ISR, isr_code, sizeof(isr_code) / sizeof(lc3word));
    write_word(A_IVT | ((IRQ_BASE | KBD_IRQ) << 1), KBD_ISR);   /* intr. vec. */

    for (;;) {
        kbd_tick();
        mem_tick();
        cpu_tick();
    }

    // int i;

    // printf("OS Code:\r\n");
    // for (i = 0; i < sizeof(os_code) / sizeof(lc3word); i++) {
    //     print_op(os_code[i]);
    // }

    // printf("\r\nISR Code:\r\n");
    // for (i = 0; i < sizeof(isr_code) / sizeof(lc3word); i++) {
    //     print_op(isr_code[i]);
    // }

    return 0;
}

static void write_word(lc3word addr, lc3word data)
{
    mem_write_nodelay(addr, data, 0xFFFF);
}

static void fill_mem(lc3word addr, const lc3word *data, int n)
{
    int i;

    for (i = 0; i < n; i++) {
        mem_write_nodelay(addr + (i << 1), data[i], 0xFFFF);
    }
}

static void usage(const char *prog_name)
{
    printf("Usage: %s [options] executable\n", prog_name);
    printf("Run '%s --help' for options.\n", prog_name);
}

static void set_nonblock(void)
{
    struct termios term;

    tcgetattr(STDIN_FILENO, &orig_termios);
    memcpy(&term, &orig_termios, sizeof(struct termios));

    atexit(reset_terminal);
    cfmakeraw(&term);
    tcsetattr(STDIN_FILENO, TCSANOW, &term);
}

static void reset_terminal(void)
{
    tcsetattr(STDIN_FILENO, TCSANOW, &orig_termios);
}
